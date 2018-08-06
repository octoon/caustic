#ifndef OCTOON_BxDF
#define OCTOON_BxDF

#include "Hammersley.h"

namespace octoon
{
	inline float fract(float t) noexcept
	{
		return t - std::floor(t);
	}

	constexpr float saturate(float t) noexcept
	{
		return std::min(1.0f, std::max(0.0f, t));
	}

	constexpr float sign(float t) noexcept
	{
		return (t > 0) ? 1.0f : -1.0f;
	}

	constexpr float lerp(float t1, float t2, float t) noexcept
	{
		return t1 * (1.0f - t) + t2 * t;
	}

	inline RadeonRays::float3 reflect(const RadeonRays::float3& L, const RadeonRays::float3& N) noexcept
	{
		return L - 2 * (RadeonRays::dot(L, N) * N);
	}

	inline RadeonRays::float3 refract(const RadeonRays::float3& L, const RadeonRays::float3& N, float ior)
	{
		float dt = RadeonRays::dot(L, N);
		float s2 = 1.0f - dt * dt;
		float st2 = ior * ior * s2;
		float cost2 = 1 - st2;
		assert(cost2 > 0.0f);
		return (L - N * dt) * ior - N * std::sqrt(cost2);
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

	RadeonRays::float3 DiffuseBRDF(const RadeonRays::float3& N, const RadeonRays::float3& L, const RadeonRays::float3& V, float roughness)
	{
		float nl = RadeonRays::dot(L, N);
		if (nl > 0)
		{
			auto H = RadeonRays::normalize(L + V);

			float vh = std::max(0.0f, RadeonRays::dot(V, L));
			float nv = std::max(0.1f, RadeonRays::dot(V, N));

			float Fd90 = (0.5f + 2 * vh * vh) * roughness;
			float FdV = 1 + (Fd90 - 1) * std::pow(1 - nv, 5);
			float FdL = 1 + (Fd90 - 1) * std::pow(1 - nl, 5);

			float brdf = FdV * FdL * (1.0f - 0.3333f * roughness);

			return RadeonRays::float3(brdf, brdf, brdf);
		}

		return 0;
	}

	RadeonRays::float3 SpecularBRDF_GGX(const RadeonRays::float3& N, const RadeonRays::float3& L, const RadeonRays::float3& V, const RadeonRays::float3& f0, float roughness)
	{
		float nl = RadeonRays::dot(L, N);
		if (nl > 0)
		{
			auto H = RadeonRays::normalize(L + V);

			float nv = std::abs(RadeonRays::dot(V, N)) + 0.1f;
			float vh = saturate(RadeonRays::dot(V, L));
			float nh = saturate(RadeonRays::dot(H, N));

			float m = roughness * roughness;
			float m2 = m * m;
			float spec = (nh * m2 - nh) * nh + 1.0f;
			float D = m2 / (spec * spec) * PI;

			float Gv = nl * (nv * (1 - m) + m);
			float Gl = nv * (nl * (1 - m) + m);
			float G = 0.5 / (Gv + Gl);

			float Fc = std::pow(1 - vh, 5);
			RadeonRays::float3 F = f0 * (1 - Fc) + RadeonRays::float3(Fc, Fc, Fc);

			return F * G * nl * (4 * nl * nv);
		}

		return 0;
	}

	RadeonRays::float3 TangentToWorld(const RadeonRays::float3& H, const RadeonRays::float3& N)
	{
		RadeonRays::float3 Y = std::abs(N.z) < 0.999f ? RadeonRays::float3(0, 0, 1) : RadeonRays::float3(1, 0, 0);
		RadeonRays::float3 X = RadeonRays::normalize(RadeonRays::cross(Y, N));
		return RadeonRays::normalize(X * H.x + cross(N, X) * H.y + N * H.z);
	}

	RadeonRays::float3 LobeDirection(const RadeonRays::float3& n, float roughness, const RadeonRays::float2& Xi)
	{
		auto H = ImportanceSampleGGX(Xi, roughness);
		return TangentToWorld(H, n);
	}

	RadeonRays::float3 CosineDirection(const RadeonRays::float3& n, const RadeonRays::float2& Xi)
	{
		auto H = CosineSampleHemisphere(Xi);
		return TangentToWorld(H, n);
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

	RadeonRays::float3 bsdf_weight(const RadeonRays::float3& V, const RadeonRays::float3& N, const RadeonRays::float3& L, const RadeonRays::float3& f0, float roughness, float ior)
	{
		if (ior > 1.0f)
			return SpecularBRDF_GGX(N, L, -V, f0, roughness);

		if (roughness < 1.0f)
			return SpecularBRDF_GGX(N, L, -V, f0, roughness);

		return DiffuseBRDF(N, L, -V, roughness);
	}
}

#endif