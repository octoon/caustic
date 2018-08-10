#ifndef OCTOON_CAUSTIC_BSDF
#define OCTOON_CAUSTIC_BSDF

#include <octoon/caustic/material.h>

namespace octoon
{
	namespace caustic
	{
		RadeonRays::float3 Disney_Sample(const RadeonRays::float3& N, const RadeonRays::float3& wi, const Material& mat, const RadeonRays::float2& Xi) noexcept;
		RadeonRays::float3 Disney_Evaluate(const RadeonRays::float3& N, const RadeonRays::float3& wi, const RadeonRays::float3& wo, const Material& mat, const RadeonRays::float2& sample) noexcept;
	}
}

#endif