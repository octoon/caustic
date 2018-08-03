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

	inline RadeonRays::float3 HemisphereSampleUniform(const RadeonRays::float2& Xi)
	{
		float phi = Xi.y * 2.0f * PI;
		float cosTheta = 1.0f - Xi.x;
		float sinTheta = sqrt(1.0f - cosTheta * cosTheta);

		return RadeonRays::float3(std::cos(phi) * sinTheta, cosTheta, std::sin(phi) * sinTheta);
	}

	inline RadeonRays::float3 HammersleySampleCos(const RadeonRays::float2& Xi)
	{
		float phi = Xi.x * 2.0f * PI;

		float cosTheta = std::sqrt(Xi.y);
		float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);

		RadeonRays::float3 H;
		H.x = std::cos(phi) * sinTheta;
		H.y = std::sin(phi) * sinTheta;
		H.z = cosTheta;

		return H;
	}

	inline RadeonRays::float3 HammersleySampleGGX(const RadeonRays::float2& Xi, float roughness)
	{
		float m = roughness * roughness;
		float m2 = m * m;
		float u = (1.0f - Xi.y) / (1.0f + (m2 - 1) * Xi.y);

		return HammersleySampleCos(RadeonRays::float2(Xi.x, u));
	}

	inline RadeonRays::float3 HammersleySampleLambert(const RadeonRays::float3& n, std::uint32_t i, std::uint32_t samplesCount, std::uint32_t seed)
	{
		auto H = HammersleySampleCos(Hammersley(i, samplesCount, seed));
		H.z = H.z * 2.0f - 1.0f;
		H += n;
		H.normalize();

		return H;
	}

	inline RadeonRays::float3 TangentToWorld(const RadeonRays::float3& N, const RadeonRays::float3& H)
	{
		RadeonRays::float3 Y = std::abs(N.z) < 0.999f ? RadeonRays::float3(0, 0, 1) : RadeonRays::float3(1, 0, 0);
		RadeonRays::float3 X = RadeonRays::cross(Y, N);
		X.normalize();
		return X * H.x + cross(N, X) * H.y + N * H.z;
	}
}

#endif