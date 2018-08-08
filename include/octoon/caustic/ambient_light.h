#ifndef OCTOON_CAUSTIC_AMBIENT_LIGHT_H_
#define OCTOON_CAUSTIC_AMBIENT_LIGHT_H_

#include <octoon/caustic/light.h>

namespace octoon
{
	namespace caustic
	{
		class AmbientLight : public Light
		{
		public:
			AmbientLight() noexcept;
			AmbientLight(const float color[3]) noexcept;
			virtual ~AmbientLight() noexcept;

		private:
			AmbientLight(const AmbientLight&) noexcept = delete;
			AmbientLight& operator=(const AmbientLight&) noexcept = delete;
		};
	}
}

#endif