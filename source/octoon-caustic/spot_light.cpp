#include <octoon/caustic/spot_light.h>
#include "math.h"
#include "disney.h"

namespace octoon
{
	namespace caustic
	{
		SpotLight::SpotLight() noexcept
			: direction_(0, 1, 0)
		{
			this->setAngle(60.0f);
		}

		SpotLight::SpotLight(const RadeonRays::float3& pos, const RadeonRays::float3& color, float angle) noexcept
			: SpotLight()
		{
			RadeonRays::matrix transform(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, pos.x, pos.y, pos.z, 1);
			this->setTransform(transform, transform);
			this->setColor(color);
			this->setAngle(angle);
		}

		SpotLight::~SpotLight() noexcept
		{
		}

		void 
		SpotLight::setAngle(float angle) noexcept
		{
			angle_ = angle;
			cosAngle_ = std::cos(angle * PI / 180.0f);
		}

		float 
		SpotLight::getAngle() const noexcept
		{
			return angle_;
		}

		void 
		SpotLight::setDirection(const RadeonRays::float3& dir) noexcept
		{
			direction_ = dir;
		}

		RadeonRays::float3 
		SpotLight::getDirection() const noexcept
		{
			return direction_;
		}

		RadeonRays::float4
		SpotLight::sample(const RadeonRays::float3& P, const RadeonRays::float3& N, const Material& mat, const RadeonRays::float2& Xi) const noexcept
		{
			auto L = this->getTranslate() - P;
			auto len = std::sqrt(RadeonRays::dot(L, L));
			L = RadeonRays::normalize(L);
			L.w = RadeonRays::dot(direction_, L) > cosAngle_ ? len : 0;

			return L;
		}

		RadeonRays::float3
		SpotLight::Li(const RadeonRays::float3& N, const RadeonRays::float3& V, const RadeonRays::float3& L, const Material& mat, const RadeonRays::float2& Xi) const noexcept
		{
			return this->getColor() * this->getColorTemperature() * Disney_Evaluate(N, V, L, mat, Xi);
		}
	}
}