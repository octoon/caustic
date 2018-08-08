#include <octoon/caustic/ambient_light.h>

namespace octoon
{
	namespace caustic
	{
		AmbientLight::AmbientLight() noexcept
		{
		}

		AmbientLight::AmbientLight(const float color[3]) noexcept
		{
			this->setColor(color);
		}

		AmbientLight::~AmbientLight() noexcept
		{
		}

		RadeonRays::float3
		AmbientLight::sample(const RadeonRays::float3& P, const RadeonRays::float3& N, const Material& mat, const RadeonRays::float2& Xi) const noexcept
		{
			return RadeonRays::float3(0, 0, 0);
		}
	}
}