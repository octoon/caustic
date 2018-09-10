#include <octoon/caustic/spot_light.h>
#include "disney.h"

namespace octoon
{
	namespace caustic
	{
		SpotLight::SpotLight() noexcept
		{
		}

		SpotLight::~SpotLight() noexcept
		{
		}

		RadeonRays::float4
		SpotLight::sample(const RadeonRays::float3& P, const RadeonRays::float3& N, const Material& mat, const RadeonRays::float2& Xi) const noexcept
		{
			auto L = this->getTranslate() - P;
			auto len = std::sqrt(RadeonRays::dot(L, L));
			L = RadeonRays::normalize(L);
			L.w = len;

			return L;
		}

		RadeonRays::float3
		SpotLight::Li(const RadeonRays::float3& N, const RadeonRays::float3& V, const RadeonRays::float3& L, const Material& mat, const RadeonRays::float2& Xi) const noexcept
		{
			return this->getColor() * this->getColorTemperature() * Disney_Evaluate(N, V, L, mat, Xi);
		}
	}
}