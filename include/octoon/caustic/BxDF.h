#ifndef OCTOON_CAUSTIC_BxDF
#define OCTOON_CAUSTIC_BxDF

#include <radeon_rays.h>
#include <octoon/caustic/material.h>

namespace octoon
{
	namespace caustic
	{
		class BxDF
		{
		public:
			BxDF() noexcept;
			virtual ~BxDF() noexcept;

			virtual RadeonRays::float3 bsdf(const RadeonRays::float3& V, RadeonRays::float3 N, const Material& mat, const RadeonRays::float2& Xi) noexcept = 0;
			virtual RadeonRays::float3 bsdf_weight(const RadeonRays::float3& V, const RadeonRays::float3& N, const RadeonRays::float3& L, const Material& mat, const RadeonRays::float2& Xi) noexcept = 0;

		private:
			BxDF(const BxDF&) = delete;
			BxDF& operator=(const BxDF&) = delete;
		};
	}
}

#endif