#ifndef OCTOON_BxDF
#define OCTOON_BxDF

#include <radeon_rays.h>
#include "Hammersley.h"

namespace octoon
{
	inline RadeonRays::float3 reflect(const RadeonRays::float3& I, const RadeonRays::float3& N) noexcept
	{
		return I - 2 * (RadeonRays::dot(I, N) * N);
	}

	float GetPhysicalLightAttenuation(const RadeonRays::float3& L, float radius = std::numeric_limits<float>::max(), float attenuationBulbSize = 1.0f)
	{
		const float invRadius = 1.0f / radius;
		float d = std::sqrt(RadeonRays::dot(L, L));
		float fadeoutFactor = std::min(1.0f, std::max(0.0f, (radius - d) * (invRadius / 0.2f)));
		d = std::max(d - attenuationBulbSize, 0.0f);
		float denom = 1.0f + d / attenuationBulbSize;
		float attenuation = fadeoutFactor * fadeoutFactor / (denom * denom);
		return attenuation;
	}

	float hash(float seed)
	{
		float noise = std::sin(seed) * 43758.5453f;
		return noise - std::floor(noise);
	};

	RadeonRays::float3 CosineDirection(const RadeonRays::float3& n, float seed)
	{
		float u = hash(78.233f + seed);
		float v = hash(10.873f + seed);

		RadeonRays::float3 H = HammersleySampleCos(RadeonRays::float2(v, u));
		H.z = H.z * 2.0f - 1.0f;
		H += n;
		H.normalize();

		return H;
	}

	RadeonRays::float3 bsdf(const RadeonRays::float3& n, std::uint32_t i, std::uint32_t samplesCount, std::uint32_t seed)
	{
		return CosineDirection(n, hash(seed * 1.0f) + float(i) / samplesCount);
	}
}

#endif