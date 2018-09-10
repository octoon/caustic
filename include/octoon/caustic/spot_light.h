#ifndef OCTOON_CAUSTIC_SPOT_LIGHT_H_
#define OCTOON_CAUSTIC_SPOT_LIGHT_H_

#include <octoon/caustic/light.h>

namespace octoon
{
	namespace caustic
	{
		class SpotLight : public Light
		{
		public:
			SpotLight() noexcept;
			SpotLight(const RadeonRays::float3& pos, const RadeonRays::float3& color, float angle = 60.0f) noexcept;
			virtual ~SpotLight() noexcept;

			void setAngle(float angle) noexcept;
			float getAngle() const noexcept;

			void setDirection(const RadeonRays::float3& dir) noexcept;
			RadeonRays::float3 getDirection() const noexcept;

			virtual RadeonRays::float3 sample(const RadeonRays::float3& P, const RadeonRays::float3& N, const Material& mat, const RadeonRays::float2& Xi) const noexcept override;
			virtual RadeonRays::float3 Li(const RadeonRays::float3& N, const RadeonRays::float3& V, const RadeonRays::float3& L, const Material& mat, const RadeonRays::float2& Xi) const noexcept override;

		private:
			SpotLight(const SpotLight&) noexcept = delete;
			SpotLight& operator=(const SpotLight&) noexcept = delete;

		private:
			float angle_;
			float cosAngle_;
			RadeonRays::float3 direction_;
		};
	}
}

#endif