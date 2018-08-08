#include <octoon/caustic/system.h>
#include <octoon/caustic/ambient_light.h>
#include <octoon/caustic/film_camera.h>
#include "montecarlo.h"

namespace octoon
{
	namespace caustic
	{
		System::System() noexcept
			: isQuitRequest_(false)
			, tileSize_(512)
		{
		}

		System::System(std::uint32_t w, std::uint32_t h) noexcept
			: System()
		{
			this->setup(w, h);
		}

		System::System(std::uint32_t w, std::uint32_t h, std::uint32_t tileSize) noexcept
			: System()
		{
			this->setup(w, h);
			this->setTileSize(tileSize);
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
			tileWidth_ = (width_ + tileSize_ - 1) / tileSize_;
			tileHeight_ = (height_ + tileSize_ - 1) / tileSize_;

			float color[3] = { 2.0f, 2.0f, 2.0f };
			float transform[4][4] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0.f, 1.f, 3.f,1 };

			auto camera = std::make_shared<FilmCamera>();
			camera->setTransform(transform, transform);

			scene_ = std::make_shared<Scene>();
			scene_->addRenderObject(std::make_shared<AmbientLight>(color));
			scene_->addRenderObject(std::move(camera));

 			thread_ = std::thread(std::bind(&System::thread, this));
		}

		void
		System::setTileSize(std::uint32_t size) noexcept
		{
			tileSize_ = size;
			tileWidth_ = (width_ + tileSize_ - 1) / tileSize_;
			tileHeight_ = (height_ + tileSize_ - 1) / tileSize_;
		}

		std::uint32_t
		System::getTileSize() const noexcept
		{
			return tileSize_;
		}

		std::future<std::uint32_t>
		System::renderTile(std::uint32_t frame, std::uint32_t tile) noexcept
		{
			auto x = tile % tileWidth_ * tileSize_;
			auto y = tile / tileWidth_ * tileSize_;

			std::packaged_task<std::uint32_t()> task([=]()
			{
				pipeline_->render(*scene_, frame,
					x, y,
					std::min(tileSize_, static_cast<std::int32_t>(width_ - x)),
					std::min(tileSize_, static_cast<std::int32_t>(height_ - y)));

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
			for (std::int32_t i = 0; i < tileWidth_ * tileHeight_; i++)
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