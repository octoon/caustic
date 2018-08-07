#ifndef OCTOON_CAUSTIC_BRDF
#define OCTOON_CAUSTIC_BRDF

#include <octoon/caustic/BxDF.h>

namespace octoon
{
	namespace caustic
	{
		class BRDF : public BxDF
		{
		public:
			BRDF() noexcept;
			virtual ~BRDF() noexcept;

			virtual RadeonRays::float3 bsdf(const RadeonRays::float3& V, RadeonRays::float3 N, const Material& mat, const RadeonRays::float2& Xi) noexcept = 0;
			virtual RadeonRays::float3 bsdf_weight(const RadeonRays::float3& V, const RadeonRays::float3& N, const RadeonRays::float3& L, const Material& mat, const RadeonRays::float2& Xi) noexcept = 0;
		};
	}
}

#endif