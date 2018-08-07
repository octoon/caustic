#ifndef OCTOON_CAUSTIC_POINT_LIGHT_H_
#define OCTOON_CAUSTIC_POINT_LIGHT_H_

#include <octoon/caustic/light.h>

namespace octoon
{
	namespace caustic
	{
		class PointLight : public Light
		{
		public:
			PointLight() noexcept;
			virtual ~PointLight() noexcept;

		private:
			PointLight(const PointLight&) noexcept;
			PointLight& operator=(const PointLight&) noexcept;
		};
	}
}

#endif