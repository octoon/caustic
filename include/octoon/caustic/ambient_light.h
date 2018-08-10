#ifndef OCTOON_CAUSTIC_AMBIENT_LIGHT_H_
#define OCTOON_CAUSTIC_AMBIENT_LIGHT_H_

#include <octoon/caustic/light.h>

namespace octoon
{
	namespace caustic
	{
		class AmbientLight : public Light
		{
		public:
			AmbientLight() noexcept;
			AmbientLight(const RadeonRays::float3& color) noexcept;
			virtual ~AmbientLight() noexcept;

			virtual RadeonRays::float4 sample(const RadeonRays::float3& P, const RadeonRays::float3& N, const Material& mat, const RadeonRays::float2& Xi) const noexcept override;

		private:
			AmbientLight(const AmbientLight&) noexcept = delete;
			AmbientLight& operator=(const AmbientLight&) noexcept = delete;
		};
	}
}

#endif