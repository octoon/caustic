#include <octoon/caustic/sphere_light.h>
#include "disney.h"
#include "math.h"

namespace octoon
{
	namespace caustic
	{
		SphereLight::SphereLight() noexcept
			: radius_(0.0f)
		{
		}

		SphereLight::SphereLight(const RadeonRays::float3& pos, const RadeonRays::float3& color) noexcept
		{
			RadeonRays::matrix transform(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, pos.x, pos.y, pos.z, 1);
			this->setTransform(transform, transform);
			this->setColor(color);
		}

		SphereLight::SphereLight(const RadeonRays::float3& pos, const RadeonRays::float3& color, float radius) noexcept
			: SphereLight(pos, color)
		{
			this->setRadius(radius);
		}

		SphereLight::~SphereLight() noexcept
		{
		}

		void 
		SphereLight::setRadius(float radius) noexcept
		{
			radius_ = radius;
		}

		float 
		SphereLight::getRadius() const noexcept
		{
			return radius_;
		}

		RadeonRays::float3
		SphereLight::sample(const RadeonRays::float3& P, const RadeonRays::float3& N, const Material& mat, const RadeonRays::float2& Xi) const noexcept
		{
			auto L = this->getTranslate() + UniformSampleSphere(Xi) * radius_ - P;
			auto len = std::sqrt(RadeonRays::dot(L, L));
			L = RadeonRays::normalize(L);
			L.w = len;

			return L;
		}

		RadeonRays::float3
		SphereLight::Li(const RadeonRays::float3& N, const RadeonRays::float3& V, const RadeonRays::float3& L, const Material& mat, const RadeonRays::float2& Xi) const noexcept
		{
			return this->getColor() * Disney_Evaluate(N, V, L, mat, Xi);
		}
	}
}