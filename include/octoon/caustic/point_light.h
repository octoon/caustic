#ifndef OCTOON_CAUSTIC_POINT_LIGHT_H_
#define OCTOON_CAUSTIC_POINT_LIGHT_H_

#include <octoon/caustic/light.h>

namespace octoon
{
	namespace caustic
	{
		class PointLight : public Light
		{
		public:
			PointLight() noexcept;
			PointLight(const RadeonRays::float3& pos, const RadeonRays::float3& color) noexcept;
			virtual ~PointLight() noexcept;

			virtual RadeonRays::float3 sample(const RadeonRays::float3& P, const RadeonRays::float3& N, const Material& mat, const RadeonRays::float2& Xi) const noexcept override;
			virtual RadeonRays::float3 Li(const RadeonRays::float3& N, const RadeonRays::float3& V, const RadeonRays::float3& L, const Material& mat, const RadeonRays::float2& Xi) const noexcept override;

		private:
			PointLight(const PointLight&) noexcept = delete;
			PointLight& operator=(const PointLight&) noexcept = delete;
		};
	}
}

#endif