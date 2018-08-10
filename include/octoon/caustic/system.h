#ifndef OCTOON_CAUSTIC_MONTECARLO_THREAD_H_
#define OCTOON_CAUSTIC_MONTECARLO_THREAD_H_

#include <future>
#include <queue>
#include <thread>

#include <octoon/caustic/pipeline.h>

namespace octoon
{
	namespace caustic
	{
		class System
		{
		public:
			System() noexcept;
			System(std::uint32_t w, std::uint32_t h) noexcept;
			System(std::uint32_t w, std::uint32_t h, std::uint32_t tileWidth, std::uint32_t tileHeight) noexcept;
			~System() noexcept;

			void setup(std::uint32_t w, std::uint32_t h) noexcept(false);

			void setTileWidth(std::uint32_t w) noexcept;
			void setTileHeight(std::uint32_t h) noexcept;

			std::uint32_t getTileWidth() const noexcept;
			std::uint32_t getTileHeight() const noexcept;

			const std::uint32_t* data() const noexcept { return pipeline_->data(); };

			bool wait_one() noexcept;

			void render(std::uint32_t frame) noexcept;
			std::future<std::uint32_t> renderTile(std::uint32_t frame, std::uint32_t tile) noexcept;
			std::future<std::uint32_t> renderFullscreen(std::uint32_t frame) noexcept;

		private:
			void thread() noexcept;

		private:
			std::uint32_t width_;
			std::uint32_t height_;
			std::unique_ptr<Pipeline> pipeline_;

			std::int32_t tileWidth_;
			std::int32_t tileHeight_;

			bool isQuitRequest_;
			std::mutex lock_;
			std::thread thread_;
			std::queue<std::packaged_task<std::uint32_t()>> task_;
			std::vector<std::future<std::uint32_t>> queues_;

			std::shared_ptr<Scene> scene_;
		};
	}
}

#endif