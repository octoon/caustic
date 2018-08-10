#ifndef OCTOON_MATH_H_
#define OCTOON_MATH_H_

#include <radeon_rays.h>
#include <cassert>

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

	template <typename T, typename U, typename V>
    T clamp(T val, U low, V high)
    {
        if (val < low) return low;
        else if (val > high) return high;
        else return val;
    }

	template<typename T>
	T frac(const T x) noexcept
	{
		return x - std::floor(x);
	}
}

#endif