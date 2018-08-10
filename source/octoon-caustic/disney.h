#ifndef OCTOON_CAUSTIC_BSDF
#define OCTOON_CAUSTIC_BSDF

#include <octoon/caustic/material.h>

namespace octoon
{
	namespace caustic
	{
		RadeonRays::float3 Disney_Sample(const RadeonRays::float3& N, const RadeonRays::float3& V, const Material& mat, const RadeonRays::float2& Xi) noexcept;
		RadeonRays::float3 Disney_Evaluate(const RadeonRays::float3& N, const RadeonRays::float3& V, const RadeonRays::float3& L, const Material& mat, const RadeonRays::float2& Xi) noexcept;
	}
}

#endif