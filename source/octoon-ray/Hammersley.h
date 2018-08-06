#ifndef OCTOON_HAMMERSLEY
#define OCTOON_HAMMERSLEY

#include <algorithm>

namespace octoon
{
	inline float fast_sin(float x)
	{
		constexpr float B = 4.0f / PI;
		constexpr float C = -4.0f / (PI * PI);
		constexpr float P = 0.225f;

		float y = B * x + C * x * (x < 0 ? -x : x);
		return P * (y * (y < 0 ? -y : y) - y) + y;
	}

	inline float fast_cos(float x)
	{
		constexpr float B = 4.0f / PI;
		constexpr float C = -4.0f / (PI * PI);
		constexpr float P = 0.225f;

		x = (x > 0) ? -x : x;
		x += PI / 2;

		return fast_sin(x);
	}

	inline float fast_rsqrt(float x) noexcept
	{
		float xhalf = 0.5f*x;
		int i = *(int*)&x;
		i = 0x5f3759df - (i >> 1);
		x = *(float*)&i;
		x = x * (1.5f - xhalf * x*x);
		x = x * (1.5f - xhalf * x*x);
		return x;
	}

	inline float fast_sqrt(float x) noexcept
	{
		return 1.0f / fast_rsqrt(x);
	}

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
		float sinTheta = fast_sqrt(1 - cosTheta * cosTheta);

		RadeonRays::float4 H;
		H.x = fast_cos(phi) * sinTheta;
		H.y = fast_sin(phi) * sinTheta;
		H.z = cosTheta;
		H.w = 1.0 / (4 * PI);

		return H;
	}

	inline RadeonRays::float3 UniformSampleHemisphere(const RadeonRays::float2& Xi)
	{
		float phi = Xi.x * 2 * PI;

		float cosTheta = Xi.y;
		float sinTheta = fast_sqrt(1.0f - cosTheta * cosTheta);

		RadeonRays::float3 H;
		H.x = fast_cos(phi) * sinTheta;
		H.y = fast_sin(phi) * sinTheta;
		H.z = cosTheta;
		H.w = 1.0f / (2 * PI);

		return H;
	}

	inline RadeonRays::float3 UniformSampleCone(const RadeonRays::float2& Xi, float CosThetaMax)
	{
		float phi = 2 * PI * Xi.x;
		float cosTheta = CosThetaMax * (1 - Xi.y) + Xi.y;
		float sinTheta = fast_sqrt(1 - cosTheta * cosTheta);

		RadeonRays::float3 H;
		H.x = sinTheta * fast_cos(phi);
		H.y = sinTheta * fast_sin(phi);
		H.z = cosTheta;
		H.w = 1.0 / (2 * PI * (1 - CosThetaMax));

		return H;
	}

	inline RadeonRays::float3 CosineSampleHemisphere(const RadeonRays::float2& Xi)
	{
		float phi = Xi.x * 2.0f * PI;

		float cosTheta = fast_sqrt(Xi.y);
		float sinTheta = fast_sqrt(1.0f - cosTheta * cosTheta);

		RadeonRays::float3 H;
		H.x = fast_cos(phi) * sinTheta;
		H.y = fast_sin(phi) * sinTheta;
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
		float sinTheta = fast_sqrt(1 - cosTheta * cosTheta);

		RadeonRays::float3 H;
		H.x = sinTheta * cos(phi);
		H.y = sinTheta * sin(phi);
		H.z = cosTheta;

		return H;
	}
}

#endif