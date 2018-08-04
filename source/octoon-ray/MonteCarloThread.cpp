#include "MonteCarloThread.h"
#include "MonteCarlo.h"

namespace octoon
{
	MonteCarloThread::MonteCarloThread() noexcept
		: isQuitRequest_(false)
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
 		thread_ = std::thread(std::bind(&MonteCarloThread::thread, this));
	}

	std::future<std::uint32_t>
	MonteCarloThread::render(std::uint32_t frame, std::uint32_t tile) noexcept
	{
		std::packaged_task<std::uint32_t()> task([=]() {  pipeline_->render(frame, tile); return y; });

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