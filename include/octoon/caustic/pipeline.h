#ifndef OCTOON_CAUSTIC_PIPELINE_H_
#define OCTOON_CAUSTIC_PIPELINE_H_

#include <octoon/caustic/render_scene.h>

namespace octoon
{
	namespace caustic
	{
		class Pipeline
		{
		public:
			Pipeline() noexcept;
			virtual ~Pipeline() noexcept;

			virtual const std::uint32_t* data() const noexcept = 0;

			virtual void render(const Camera& camera, std::uint32_t frame, std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h) noexcept = 0;
		};
	}
}

#endif