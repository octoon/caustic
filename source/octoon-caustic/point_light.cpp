#include <octoon/caustic/point_light.h>

namespace octoon
{
	namespace caustic
	{
		PointLight::PointLight() noexcept
		{
		}

		PointLight::~PointLight() noexcept
		{
		}

		void 
		PointLight::sample(const float ro[3], const float norm[3], const Material& mat, const float Xi[2], float L[3]) const noexcept
		{
			auto transform = this->getTransform();
			auto x = transform[3 * 4 + 0];
			auto y = transform[3 * 4 + 1];
			auto z = transform[3 * 4 + 2];

			L[0] = ro[0] - x;
			L[1] = ro[1] - y;
			L[2] = ro[2] - z;
		}
	}
}