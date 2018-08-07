#ifndef OCTOON_CAUSTIC_SPOT_LIGHT_H_
#define OCTOON_CAUSTIC_SPOT_LIGHT_H_

#include <octoon/caustic/light.h>

namespace octoon
{
	namespace caustic
	{
		class SpotLight : public Light
		{
		public:
			SpotLight() noexcept;
			virtual ~SpotLight() noexcept;

		private:
			SpotLight(const SpotLight&) noexcept;
			SpotLight& operator=(const SpotLight&) noexcept;
		};
	}
}

#endif