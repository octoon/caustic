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

	inline RadeonRays::float3 refract(const RadeonRays::float3& I, const RadeonRays::float3& normal, float ior)
	{
		float dt = RadeonRays::dot(I, normal);
		float s2 = 1.0f - dt * dt;
		float st2 = ior * ior * s2;
		float cost2 = 1 - st2;
		return (I - normal * dt) * ior - normal * std::sqrt(cost2);
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

	float SmithJointApprox(float a2, float NoV, float NoL)
	{
		float a = sqrt(a2);
		float Vis_SmithV = NoL * (NoV * (1 - a) + a);
		float Vis_SmithL = NoV * (NoL * (1 - a) + a);
		return 0.5f * 1.0f / (Vis_SmithV + Vis_SmithL);
	}

	float DiffuseBRDF(const RadeonRays::float3& N, const RadeonRays::float3& L, const RadeonRays::float3& V, float roughness)
	{
		float nl = RadeonRays::dot(L, N);
		if (nl > 0)
		{
			auto H = RadeonRays::normalize(L + V);

			float vh = RadeonRays::dot(V, L);
			float nv = RadeonRays::dot(V, N);

			float FD90 = (0.5f + 2 * vh * vh) * roughness;
			float FdV = 1 + (FD90 - 1) * std::pow(1 - nv, 5);
			float FdL = 1 + (FD90 - 1) * std::pow(1 - nl, 5);

			return std::max(0.0f, FdV * FdL * (1.0f - 0.3333f * roughness));
		}

		return 0;
	}

	float SpecularBRDF_GGX(const RadeonRays::float3& N, const RadeonRays::float3& L, const RadeonRays::float3& V, float roughness)
	{
		float nl = RadeonRays::dot(L, N);
		if (nl > 0)
		{
			auto H = RadeonRays::normalize(L + V);

			float nv = RadeonRays::dot(V, N);
			float vh = RadeonRays::dot(V, L);
			float nh = RadeonRays::dot(H, N);

			float G = SmithJointApprox(std::pow(roughness, 4), nv, nl);
			float Fc = std::pow(1 - vh, 5);
			float F = (1 - Fc) + Fc;

			return std::max(0.0f, nl * F * G * (4 * vh / nh));
		}

		return 0;
	}

	RadeonRays::float3 TangentToWorld(const RadeonRays::float3& H, const RadeonRays::float3& N)
	{
		RadeonRays::float3 Y = std::abs(N.z) < 0.999f ? RadeonRays::float3(0, 0, 1) : RadeonRays::float3(1, 0, 0);
		RadeonRays::float3 X = RadeonRays::normalize(RadeonRays::cross(Y, N));
		return X * H.x + cross(N, X) * H.y + N * H.z;
	}

	RadeonRays::float3 LobeDirection(const RadeonRays::float3& n, float roughness, const RadeonRays::float2& Xi)
	{
		auto H = ImportanceSampleGGX(Xi, roughness);
		return TangentToWorld(H, n);
	}

	RadeonRays::float3 CosineDirection(const RadeonRays::float3& n, const RadeonRays::float2& Xi)
	{
		float a = Xi.x * 2.0f * PI;
		float u = Xi.y * 2.0f - 1.0f;
		float sinTheta = std::sqrt(1.0f - u * u);

		return RadeonRays::normalize(n + RadeonRays::float3(cos(a) * sinTheta, sin(a) * sinTheta, u));
	}

	RadeonRays::float3 CosineDirection(const RadeonRays::float3& n, std::uint32_t i, std::uint32_t samplesCount)
	{
		auto H = CosineSampleHemisphere(Hammersley(i, samplesCount));
		return TangentToWorld(H, n);
	}

	RadeonRays::float3 bsdf(const RadeonRays::float3& V, const RadeonRays::float3& N, float roughness, float ior, const RadeonRays::float2& Xi)
	{
		if (ior > 1.0f)
			return LobeDirection(RadeonRays::normalize(refract(V, N, 1.0f / ior)), roughness, Xi);

		if (roughness < 1.0f)
			return LobeDirection(RadeonRays::normalize(reflect(V, N)), roughness, Xi);

		return CosineDirection(N, Xi);
	}

	float rand(std::uint64_t seed)
	{
		float hash = (std::sin(seed) * 43758.5453123f);
		return hash - std::floor(hash);
	}
}

#endif