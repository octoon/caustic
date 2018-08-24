#include <octoon/caustic/system.h>
#include <octoon/caustic/ambient_light.h>
#include <octoon/caustic/film_camera.h>
#include <octoon/caustic/point_light.h>
#include <octoon/caustic/sphere_light.h>
#include "montecarlo.h"

namespace octoon
{
	namespace caustic
	{
		System::System() noexcept
			: isQuitRequest_(false)
			, tileWidth_(512)
			, tileHeight_(512)
		{
		}

		System::System(std::uint32_t w, std::uint32_t h) noexcept
			: System()
		{
			this->setup(w, h);
		}

		System::System(std::uint32_t w, std::uint32_t h, std::uint32_t tileWidth, std::uint32_t tileHeight) noexcept
			: System()
		{
			this->setup(w, h);
			this->setTileWidth(tileWidth);
			this->setTileHeight(tileHeight);
		}

		System::~System()
		{
			isQuitRequest_ = true;
			thread_.join();
		}

		void
		System::setup(std::uint32_t w, std::uint32_t h) noexcept(false)
		{
			width_ = w;
			height_ = h;

			RadeonRays::matrix transform(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0.f, 1.f, 3.f, 1);

			auto camera = std::make_shared<FilmCamera>();
			camera->setTransform(transform, transform);

			auto sphereLight = std::make_shared<SphereLight>(RadeonRays::float3(0.0f, 1.5f, 0.0f), RadeonRays::float3(28.0f, 28.0f, 28.0f));
			sphereLight->setRadius(0.1f);
			sphereLight->setTemperature(6000);

			scene_ = std::make_shared<Scene>();
			scene_->addRenderObject(std::move(sphereLight));
			scene_->addRenderObject(std::move(camera));

 			thread_ = std::thread(std::bind(&System::thread, this));
		}

		void
		System::setTileWidth(std::uint32_t size) noexcept
		{
			tileWidth_ = size;
		}

		void
		System::setTileHeight(std::uint32_t size) noexcept
		{
			tileHeight_ = size;
		}

		std::uint32_t
		System::getTileWidth() const noexcept
		{
			return tileWidth_;
		}

		std::uint32_t
		System::getTileHeight() const noexcept
		{
			return tileHeight_;
		}

		std::future<std::uint32_t>
		System::renderTile(std::uint32_t frame, std::uint32_t tile) noexcept
		{
			auto w = (width_ + tileWidth_ - 1) / tileWidth_;
			auto x = tile % w * tileWidth_;
			auto y = tile / w * tileHeight_;

			std::packaged_task<std::uint32_t()> task([=]()
			{
				pipeline_->render(*scene_, frame,
					x, y,
					std::min<std::uint32_t>(tileWidth_, width_ - x),
					std::min<std::uint32_t>(tileHeight_, height_ - y));

				return tile;
			});

			auto f = task.get_future();

			lock_.lock();
			task_.push(std::move(task));
			lock_.unlock();

			return std::move(f);
		}

		std::future<std::uint32_t>
		System::renderFullscreen(std::uint32_t frame) noexcept
		{
			std::packaged_task<std::uint32_t()> task([=]()
			{
				pipeline_->render(*scene_, frame,
					0, 0,
					width_, height_
				);

				return 0;
			});

			auto f = task.get_future();

			lock_.lock();
			task_.push(std::move(task));
			lock_.unlock();

			return std::move(f);
		}

		void
		System::render(std::uint32_t frame) noexcept
		{
			auto w = (width_ + tileWidth_ - 1) / tileWidth_;
			auto h = (height_ + tileHeight_ - 1) / tileHeight_;

			for (std::int32_t i = 0; i < w * h; i++)
				queues_.push_back(this->renderTile(frame, i));
		}

		bool
		System::wait_one() noexcept
		{
			if (!queues_.empty())
			{
				queues_.front().wait();
				queues_.erase(queues_.begin());
			}

			return !queues_.empty();
		}

		void
		System::thread() noexcept
		{
			pipeline_ = std::make_unique<MonteCarlo>(width_, height_);

			while (!isQuitRequest_)
			{
				if (!task_.empty())
				{
					lock_.lock();
					auto task = std::move(task_.front());
					task_.pop();
					lock_.unlock();

					task();
				}
			}
		}
	}
}