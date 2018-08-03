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

	inline RadeonRays::float3 refract(const RadeonRays::float3& I, const RadeonRays::float3& normal, float refractRatio)
	{
		RadeonRays::float3 inVec = I;
		inVec.normalize();
		float dt = RadeonRays::dot(inVec, normal);
		float s2 = 1.0 - dt * dt;
		float st2 = refractRatio * refractRatio * s2;
		float cost2 = 1 - st2;
		return (inVec - normal * dt) * refractRatio - normal * std::sqrt(cost2);
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

	RadeonRays::float3 bsdf(const RadeonRays::float3& rd, const RadeonRays::float3& n, float shininess, float ior, std::uint32_t i, std::uint32_t samplesCount, std::uint32_t seed)
	{
		if (shininess > 0) 
		{
			if (shininess > hash(seed + float(i) / samplesCount) * 2049.0f)
				return RadeonRays::normalize(reflect(rd, n));
		}

		if (ior > 1.0f)
			return RadeonRays::normalize(refract(rd, n, ior));

		return CosineDirection(n, hash(seed) + float(i) / samplesCount);
	}
}

#endif