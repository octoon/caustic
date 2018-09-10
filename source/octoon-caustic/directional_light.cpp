#include <octoon/caustic/directional_light.h>
#include "disney.h"

namespace octoon
{
	namespace caustic
	{
		DirectionalLight::DirectionalLight() noexcept
			: direction_(0, 1, 0)
		{
		}

		DirectionalLight::DirectionalLight(const RadeonRays::float3& dir, const RadeonRays::float3& color) noexcept
		{
			this->setColor(color);
			this->direction_ = RadeonRays::normalize(dir);
		}

		DirectionalLight::~DirectionalLight() noexcept
		{
		}
		
		void 
		DirectionalLight::setDirection(const RadeonRays::float3& dir) noexcept
		{
			direction_ = dir;
		}

		RadeonRays::float3 
		DirectionalLight::getDirection() const noexcept
		{
			return direction_;
		}

		RadeonRays::float4
		DirectionalLight::sample(const RadeonRays::float3& P, const RadeonRays::float3& N, const Material& mat, const RadeonRays::float2& Xi) const noexcept
		{
			RadeonRays::float4 L;
			L = this->direction_;
			L.w = std::numeric_limits<float>::max();

			return L;
		}

		RadeonRays::float3
		DirectionalLight::Li(const RadeonRays::float3& N, const RadeonRays::float3& V, const RadeonRays::float3& L, const Material& mat, const RadeonRays::float2& Xi) const noexcept
		{
			return this->getColor() * this->getColorTemperature() * Disney_Evaluate(N, V, L, mat, Xi);
		}
	}
}