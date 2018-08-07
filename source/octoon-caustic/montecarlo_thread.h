#ifndef OCTOON_CAUSTIC_MONTECARLO_THREAD_H_
#define OCTOON_CAUSTIC_MONTECARLO_THREAD_H_

#include <future>
#include <queue>
#include <thread>

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

		void setTileSize(std::uint32_t size) noexcept;
		std::uint32_t getTileSize() const noexcept;

		const std::uint32_t* data() const noexcept { return pipeline_->data(); };

		std::future<std::uint32_t> renderTile(std::uint32_t frame, std::uint32_t tile) noexcept;
		std::future<std::uint32_t> renderFullscreen(std::uint32_t frame) noexcept;

	private:
		void thread() noexcept;

	private:
		std::uint32_t width_;
		std::uint32_t height_;
		std::unique_ptr<MonteCarlo> pipeline_;

		std::int32_t tileSize_;
		std::int32_t tileWidth_;
		std::int32_t tileHeight_;

		bool isQuitRequest_;
		std::mutex lock_;
		std::thread thread_;
		std::queue<std::packaged_task<std::uint32_t()>> task_;
	};
}

#endif