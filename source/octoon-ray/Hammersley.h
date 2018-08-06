#ifndef OCTOON_HAMMERSLEY
#define OCTOON_HAMMERSLEY

#include <radeon_rays.h>

namespace octoon
{
	inline std::uint32_t ReverseBits32(std::uint32_t bits)
	{
		bits = (bits << 16) | (bits >> 16);
		bits = ((bits & 0x00ff00ff) << 8) | ((bits & 0xff00ff00) >> 8);
		bits = ((bits & 0x0f0f0f0f) << 4) | ((bits & 0xf0f0f0f0) >> 4);
		bits = ((bits & 0x33333333) << 2) | ((bits & 0xcccccccc) >> 2);
		bits = ((bits & 0x55555555) << 1) | ((bits & 0xaaaaaaaa) >> 1);
		return bits;
	}

	inline RadeonRays::float2 Hammersley(std::uint32_t i, std::uint32_t samplesCount)
	{
		float E1 = (float)i / samplesCount;
		float E2 = ReverseBits32(i) * 2.3283064365386963e-10f;
		return RadeonRays::float2(E1, E2);
	}

	inline RadeonRays::float2 Hammersley(std::uint32_t i, std::uint32_t samplesCount, std::uint32_t seed)
	{
		float E1 = (float)i / samplesCount + float(seed & 0xffff) / (1 << 16);
		float E2 = (ReverseBits32(i) ^ seed) * 2.3283064365386963e-10f;
		return RadeonRays::float2(E1 - std::floor(E1), E2);
	}

	inline RadeonRays::float4 UniformSampleSphere(const RadeonRays::float2& Xi)
	{
		float phi = 2 * PI * Xi.x;

		float cosTheta = 1 - 2 * Xi.y;
		float sinTheta = std::sqrt(1 - cosTheta * cosTheta);

		RadeonRays::float4 H;
		H.x = std::cos(phi) * sinTheta;
		H.y = std::sin(phi) * sinTheta;
		H.z = cosTheta;
		H.w = 1.0 / (4 * PI);

		return H;
	}

	inline RadeonRays::float3 UniformSampleHemisphere(const RadeonRays::float2& Xi)
	{
		float phi = Xi.x * 2 * PI;

		float cosTheta = Xi.y;
		float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);

		RadeonRays::float3 H;
		H.x = std::cos(phi) * sinTheta;
		H.y = std::sin(phi) * sinTheta;
		H.z = cosTheta;
		H.w = 1.0f / (2 * PI);

		return H;
	}

	inline RadeonRays::float3 UniformSampleCone(const RadeonRays::float2& Xi, float CosThetaMax)
	{
		float Phi = 2 * PI * Xi.x;
		float CosTheta = CosThetaMax * (1 - Xi.y) + Xi.y;
		float SinTheta = std::sqrt(1 - CosTheta * CosTheta);

		RadeonRays::float3 H;
		H.x = SinTheta * cos(Phi);
		H.y = SinTheta * sin(Phi);
		H.z = CosTheta;
		H.w = 1.0 / (2 * PI * (1 - CosThetaMax));

		return H;
	}

	inline RadeonRays::float3 CosineSampleHemisphere(const RadeonRays::float2& Xi)
	{
		float phi = Xi.x * 2.0f * PI;

		float cosTheta = std::sqrt(Xi.y);
		float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);

		RadeonRays::float3 H;
		H.x = std::cos(phi) * sinTheta;
		H.y = std::sin(phi) * sinTheta;
		H.z = cosTheta;
		H.w = cosTheta * (1 / PI);

		return H;
	}

	inline RadeonRays::float3 ImportanceSampleGGX(const RadeonRays::float2& Xi, float roughness)
	{
		float m = roughness * roughness;
		float m2 = m * m;
		float u = (1.0f - Xi.y) / (1.0f + (m2 - 1) * Xi.y);

		return CosineSampleHemisphere(RadeonRays::float2(Xi.x, u));
	}

	inline RadeonRays::float3 ImportanceSampleBlinn(const RadeonRays::float2& Xi, float a2)
	{
		float phi = Xi.x * 2.0f * PI;

		float n = 2 / a2 - 2;
		float cosTheta = std::pow(Xi.y, 1 / (n + 1));
		float sinTheta = std::sqrt(1 - cosTheta * cosTheta);

		RadeonRays::float3 H;
		H.x = sinTheta * cos(phi);
		H.y = sinTheta * sin(phi);
		H.z = cosTheta;

		return H;
	}
}

#endif