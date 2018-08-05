#include "MonteCarloThread.h"
#include "MonteCarlo.h"

#include <algorithm>

namespace octoon
{
	MonteCarloThread::MonteCarloThread() noexcept
		: isQuitRequest_(false)
		, tileSize_(64)
	{
	}

	MonteCarloThread::MonteCarloThread(std::uint32_t w, std::uint32_t h) noexcept
		: MonteCarloThread()
	{
		this->setup(w, h);
	}

	MonteCarloThread::~MonteCarloThread()
	{
		isQuitRequest_ = true;
		thread_.join();
	}

	void
	MonteCarloThread::setup(std::uint32_t w, std::uint32_t h) noexcept(false)
	{
		width_ = w;
		height_ = h;
		tileWidth_ = width_ / tileSize_;
		tileHeight_ = height_ / tileSize_;
 		thread_ = std::thread(std::bind(&MonteCarloThread::thread, this));
	}

	void
	MonteCarloThread::setTileSize(std::uint32_t size) noexcept
	{
		tileSize_ = size;
		tileWidth_ = width_ / tileSize_;
		tileHeight_ = height_ / tileSize_;
	}

	std::uint32_t
	MonteCarloThread::getTileSize() const noexcept
	{
		return tileSize_;
	}

	std::future<std::uint32_t>
	MonteCarloThread::render(std::uint32_t frame, std::uint32_t tile) noexcept
	{
		auto x = tile % tileWidth_ * tileSize_;
		auto y = tile / tileWidth_ * tileSize_;

		std::packaged_task<std::uint32_t()> task([=]()
		{
			pipeline_->render(frame,
				RadeonRays::int2(x, y),
				RadeonRays::int2(
					std::min(tileSize_, static_cast<std::int32_t>(width_ - x)),
					std::min(tileSize_, static_cast<std::int32_t>(height_ - y))
				)
			);

			return tile;
		});

		auto f = task.get_future();

		lock_.lock();
		task_.push(std::move(task));
		lock_.unlock();

		return std::move(f);
	}

	void
	MonteCarloThread::thread() noexcept
	{
		pipeline_ = std::make_unique<octoon::MonteCarlo>(width_, height_);
		pipeline_->setup(width_, height_);

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