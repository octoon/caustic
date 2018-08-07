#ifndef OCTOON_CAUSTIC_SPHERE_LIGHT_H_
#define OCTOON_CAUSTIC_SPHERE_LIGHT_H_

#include <octoon/caustic/light.h>

namespace octoon
{
	namespace caustic
	{
		class SphereLight : public Light
		{
		public:
			SphereLight() noexcept;
			virtual ~SphereLight() noexcept;

		private:
			SphereLight(const SphereLight&) noexcept = delete;
			SphereLight& operator=(const SphereLight&) noexcept = delete;
		};
	}
}

#endif