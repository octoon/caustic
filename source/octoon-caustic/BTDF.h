#ifndef OCTOON_CAUSTIC_BTDF
#define OCTOON_CAUSTIC_BTDF

#include <octoon/caustic/BxDF.h>

namespace octoon
{
	namespace caustic
	{
		class BTDF : public BxDF
		{
		public:
			BTDF() noexcept;
			virtual ~BTDF() noexcept;

			virtual RadeonRays::float3 sample(const RadeonRays::float3& V, RadeonRays::float3 N, const Material& mat, const RadeonRays::float2& Xi) noexcept = 0;
			virtual RadeonRays::float3 sample_weight(const RadeonRays::float3& V, const RadeonRays::float3& N, const RadeonRays::float3& L, const Material& mat, const RadeonRays::float2& Xi) noexcept = 0;
		};
	}
}

#endif