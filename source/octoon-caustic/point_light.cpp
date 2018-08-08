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

		RadeonRays::float3
		PointLight::sample(const RadeonRays::float3& P, const RadeonRays::float3& N, const Material& mat, const RadeonRays::float2& Xi) const noexcept
		{
			auto transform = this->getTransform();
			auto x = transform[3 * 4 + 0];
			auto y = transform[3 * 4 + 1];
			auto z = transform[3 * 4 + 2];

			return RadeonRays::float3(P[0] - x, P[1] - y, P[2] - z);
		}
	}
}