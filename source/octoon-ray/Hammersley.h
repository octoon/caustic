#ifndef OCTOON_HAMMERSLEY
#define OCTOON_HAMMERSLEY

#include <algorithm>

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

	static const unsigned short FaurePermutation[5 * 5 * 5] = { 0, 75, 50, 25, 100, 15, 90, 65, 40, 115, 10, 85, 60, 35, 110, 5, 80, 55,
		30, 105, 20, 95, 70, 45, 120, 3, 78, 53, 28, 103, 18, 93, 68, 43, 118, 13, 88, 63, 38, 113, 8, 83, 58, 33, 108,
		23, 98, 73, 48, 123, 2, 77, 52, 27, 102, 17, 92, 67, 42, 117, 12, 87, 62, 37, 112, 7, 82, 57, 32, 107, 22, 97,
		72, 47, 122, 1, 76, 51, 26, 101, 16, 91, 66, 41, 116, 11, 86, 61, 36, 111, 6, 81, 56, 31, 106, 21, 96, 71, 46,
		121, 4, 79, 54, 29, 104, 19, 94, 69, 44, 119, 14, 89, 64, 39, 114, 9, 84, 59, 34, 109, 24, 99, 74, 49, 124 };
	double Halton5(const unsigned Index)
	{
		// 依次提取0-2,3-5,6-8，9-11位的digits左右翻转并移到小数点右边
		return (FaurePermutation[Index % 125u] * 1953125u + FaurePermutation[(Index / 125u) % 125u] * 15625u +
			FaurePermutation[(Index / 15625u) % 125u] * 125u +
			FaurePermutation[(Index / 1953125u) % 125u]) * (0x1.fffffep-1 / 244140625u);
	}
}

#endif