#ifndef OCTOON_CAUSTIC_MATH_H_
#define OCTOON_CAUSTIC_MATH_H_

#include <radeon_rays.h>
#include <cassert>

namespace octoon
{
	namespace caustic
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

		constexpr float pow5(float x) noexcept
		{
			float x2 = x * x;
			return x2 * x2 * x;
		}

		inline RadeonRays::float3 reflect(const RadeonRays::float3& L, const RadeonRays::float3& N) noexcept
		{
			return L + 2 * (std::abs(RadeonRays::dot(L, N)) * N);
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

		inline float fast_sin(float x)
		{
			assert(x >= -PI && x <= PI);

			constexpr float B = 4.0f / PI;
			constexpr float C = -4.0f / (PI * PI);
			constexpr float P = 0.225f;

			float y = B * x + C * x * (x < 0 ? -x : x);
			return P * (y * (y < 0 ? -y : y) - y) + y;
		}

		inline float fast_cos(float x)
		{
			assert(x >= -PI && x <= PI);

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

		template <typename T, typename U, typename V>
		inline T clamp(T val, U low, V high)
		{
			if (val < low) return low;
			else if (val > high) return high;
			else return val;
		}

		template<typename T>
		inline T frac(const T x) noexcept
		{
			return x - std::floor(x);
		}

		inline float luminance(const RadeonRays::float3& rgb)
		{
			return RadeonRays::dot(rgb, RadeonRays::float3(0.299f, 0.587f, 0.114f));
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
			float phi = 2 * PI * Xi.x;
			float cosTheta = CosThetaMax * (1 - Xi.y) + Xi.y;
			float sinTheta = std::sqrt(1 - cosTheta * cosTheta);

			RadeonRays::float3 H;
			H.x = sinTheta * std::cos(phi);
			H.y = sinTheta * std::sin(phi);
			H.z = cosTheta;
			H.w = 1.0f / (2 * PI * (1 - CosThetaMax));

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
			H.x = sinTheta * std::cos(phi);
			H.y = sinTheta * std::sin(phi);
			H.z = cosTheta;

			return H;
		}

		inline RadeonRays::float3 TangentToWorld(const RadeonRays::float3& H, const RadeonRays::float3& N)
		{
			RadeonRays::float3 Y = std::abs(N.z) < 0.999f ? RadeonRays::float3(0, 0, 1) : RadeonRays::float3(1, 0, 0);
			RadeonRays::float3 X = RadeonRays::normalize(RadeonRays::cross(Y, N));
			return RadeonRays::normalize(X * H.x + RadeonRays::cross(N, X) * H.y + N * H.z);
		}
	}
}

#endif