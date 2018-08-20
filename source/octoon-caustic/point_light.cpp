#include <octoon/caustic/point_light.h>
#include "disney.h"

namespace octoon
{
	namespace caustic
	{
		PointLight::PointLight() noexcept
		{
		}

		PointLight::PointLight(const RadeonRays::float3& pos, const RadeonRays::float3& color) noexcept
		{
			RadeonRays::matrix transform(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, pos.x, pos.y, pos.z, 1);
			this->setTransform(transform, transform);
			this->setColor(color);
		}

		PointLight::~PointLight() noexcept
		{
		}

		RadeonRays::float3
		PointLight::sample(const RadeonRays::float3& P, const RadeonRays::float3& N, const Material& mat, const RadeonRays::float2& Xi) const noexcept
		{
			auto L = this->getTranslate() - P;
			auto len = std::sqrt(RadeonRays::dot(L, L));
			L = RadeonRays::normalize(L);
			L.w = len;

			return L;
		}

		RadeonRays::float3
		PointLight::Li(const RadeonRays::float3& N, const RadeonRays::float3& V, const RadeonRays::float3& L, const Material& mat, const RadeonRays::float2& Xi) const noexcept
		{
			return this->getColor() * Disney_Evaluate(N, V, L, mat, Xi);
		}
	}
}