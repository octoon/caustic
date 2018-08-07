#ifndef OCTOON_CAUSTIC_BSDF
#define OCTOON_CAUSTIC_BSDF

#include <octoon/caustic/BxDF.h>

namespace octoon
{
	namespace caustic
	{
		class BSDF : public BxDF
		{
		public:
			BSDF() noexcept;
			virtual ~BSDF() noexcept;

			virtual RadeonRays::float3 bsdf(const RadeonRays::float3& V, RadeonRays::float3 N, const Material& mat, const RadeonRays::float2& Xi) noexcept;
			virtual RadeonRays::float3 bsdf_weight(const RadeonRays::float3& V, const RadeonRays::float3& N, const RadeonRays::float3& L, const Material& mat, const RadeonRays::float2& Xi) noexcept;
		};
	}
}

#endif