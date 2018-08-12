#include <octoon/caustic/light.h>

namespace octoon
{
	namespace caustic
	{
		Light::Light() noexcept
		{
			color_[0] = 0.0f;
			color_[1] = 0.0f;
			color_[2] = 0.0f;
		}

		Light::~Light() noexcept
		{
		}

		void
		Light::setColor(const RadeonRays::float3& color) noexcept
		{
			color_ = color;
		}

		const RadeonRays::float3&
		Light::getColor() const noexcept
		{
			return color_;
		}

		RadeonRays::float3
		Light::sample(const RadeonRays::float3& P, const RadeonRays::float3& N, const Material& mat, const RadeonRays::float2& Xi) const noexcept
		{
			return RadeonRays::float3(0, 0, 0);
		}

		RadeonRays::float3
		Light::Li(const RadeonRays::float3& N, const RadeonRays::float3& V, const RadeonRays::float3& L, const Material& mat, const RadeonRays::float2& Xi) const noexcept
		{
			return RadeonRays::float3(1, 1, 1);
		}
	}
}