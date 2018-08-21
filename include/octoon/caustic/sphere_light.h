#ifndef OCTOON_CAUSTIC_SPHERE_LIGHT_H_
#define OCTOON_CAUSTIC_SPHERE_LIGHT_H_

#include <octoon/caustic/light.h>

namespace octoon
{
	namespace caustic
	{
		class SphereLight : public Light
		{
		public:
			SphereLight() noexcept;
			SphereLight(const RadeonRays::float3& pos, const RadeonRays::float3& color) noexcept;
			SphereLight(const RadeonRays::float3& pos, const RadeonRays::float3& color, float radius) noexcept;
			virtual ~SphereLight() noexcept;

			void setRadius(float radius) noexcept;
			float getRadius() const noexcept;

			virtual RadeonRays::float3 sample(const RadeonRays::float3& P, const RadeonRays::float3& N, const Material& mat, const RadeonRays::float2& Xi) const noexcept override;
			virtual RadeonRays::float3 Li(const RadeonRays::float3& N, const RadeonRays::float3& V, const RadeonRays::float3& L, const Material& mat, const RadeonRays::float2& Xi) const noexcept override;

		private:
			SphereLight(const SphereLight&) noexcept = delete;
			SphereLight& operator=(const SphereLight&) noexcept = delete;

		private:
			float radius_;
		};
	}
}

#endif