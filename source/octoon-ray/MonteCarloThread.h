#ifndef OCTOON_MONTECARLO_THREAD_H_
#define OCTOON_MONTECARLO_THREAD_H_

#include "MonteCarlo.h"

namespace octoon
{
	class MonteCarloThread
	{
	public:
		MonteCarloThread() noexcept;
		MonteCarloThread(std::uint32_t w, std::uint32_t h) noexcept;
		~MonteCarloThread() noexcept;

		void setup(std::uint32_t w, std::uint32_t h) noexcept(false);

		const std::uint32_t* raw_data(std::uint32_t y) const noexcept { return pipeline_->raw_data(y); };

		std::future<std::uint32_t> render(std::uint32_t y, std::uint32_t frame) noexcept;

	private:
		void thread() noexcept;

	private:
		std::uint32_t width_;
		std::uint32_t height_;
		std::unique_ptr<MonteCarlo> pipeline_;

		bool isQuitRequest_;
		std::mutex lock_;
		std::thread thread_;
		std::queue<std::packaged_task<std::uint32_t()>> task_;
	};
}

#endif