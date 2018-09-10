#ifndef OCTOON_CAUSTIC_DIRECTIONAL_LIGHT_H_
#define OCTOON_CAUSTIC_DIRECTIONAL_LIGHT_H_

#include <octoon/caustic/light.h>

namespace octoon
{
	namespace caustic
	{
		class DirectionalLight : public Light
		{
		public:
			DirectionalLight() noexcept;
			DirectionalLight(const RadeonRays::float3& direction, const RadeonRays::float3& color) noexcept;
			virtual ~DirectionalLight() noexcept;

			void setDirection(const RadeonRays::float3& dir) noexcept;
			RadeonRays::float3 getDirection() const noexcept;

			virtual RadeonRays::float4 sample(const RadeonRays::float3& P, const RadeonRays::float3& N, const Material& mat, const RadeonRays::float2& Xi) const noexcept override;
			virtual RadeonRays::float3 Li(const RadeonRays::float3& N, const RadeonRays::float3& V, const RadeonRays::float3& L, const Material& mat, const RadeonRays::float2& Xi) const noexcept override;

		private:
			DirectionalLight(const DirectionalLight&) noexcept = delete;
			DirectionalLight& operator=(const DirectionalLight&) noexcept = delete;

		private:
			RadeonRays::float3 direction_;
		};
	}
}

#endif