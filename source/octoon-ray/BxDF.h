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
		float dt = RadeonRays::dot(I, normal);
		float s2 = 1.0 - dt * dt;
		float st2 = refractRatio * refractRatio * s2;
		float cost2 = 1 - st2;
		return (I - normal * dt) * refractRatio - normal * std::sqrt(cost2);
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

	RadeonRays::float3 LobeDirection(const RadeonRays::float3& n, float roughness, std::uint32_t i, std::uint32_t samplesCount)
	{
		auto H = ImportanceSampleGGX(Hammersley(i, samplesCount), roughness);
		return TangentToWorld(H, n);
	}

	RadeonRays::float3 CosineDirection(const RadeonRays::float3& n, std::uint32_t i, std::uint32_t samplesCount)
	{
		RadeonRays::float3 H = CosineSampleHemisphere(Hammersley(i, samplesCount));
		return TangentToWorld(H, n);
	}

	RadeonRays::float3 bsdf(const RadeonRays::float3& rd, const RadeonRays::float3& n, float roughness, float ior, std::uint32_t i, std::uint32_t samplesCount, std::uint32_t seed)
	{
		if (roughness > 0)
		{
			auto R = RadeonRays::normalize(reflect(rd, n));
			return LobeDirection(R, i, samplesCount, roughness);
		}

		if (ior > 1.0f)
		{
			return RadeonRays::normalize(refract(rd, n, ior));
		}

		return CosineDirection(n, i, samplesCount);
	}
}

#endif