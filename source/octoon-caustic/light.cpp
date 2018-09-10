#include <octoon/caustic/light.h>
#include "math.h"

namespace octoon
{
	namespace caustic
	{
		Light::Light() noexcept
			: kelvin_(6500)
			, temperature_(RadeonRays::float3(1,1,1))
		{
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

		void 
		Light::setTemperature(float kelvin) noexcept
		{
			auto ColorTemperature = [](float kelvin)
			{
				// http://www.tannerhelland.com/4435/convert-temperature-rgb-algorithm-code/
				float temp = kelvin / 100;
				float temp60 = std::max(0.0f, temp - 60);
				float red = temp <= 66 ? 255 : 329.698727446f * pow(temp60, -0.1332047592f);
				float green = temp <= 66 ? 99.4708025861f * log(temp) - 161.1195681661f : 288.1221695283f * pow(temp60, -0.0755148492f);
				float blue = temp >= 66 ? 255 : (temp <= 19 ? 0 : 138.5177312231f * log(temp - 10) - 305.0447927307f);

				red = saturate(std::pow(red / 255.0f, 2.2f));
				green = saturate(std::pow(green / 255.0f, 2.2f));
				blue = saturate(std::pow(blue / 255.0f, 2.2f));

				return RadeonRays::float3(red, green, blue);
			};

			if (kelvin_ != kelvin)
			{
				temperature_ = ColorTemperature(kelvin);
				kelvin_ = kelvin;
			}
		}

		float 
		Light::getTemperature() const noexcept
		{
			return kelvin_;
		}

		const RadeonRays::float3&
		Light::getColorTemperature() const noexcept
		{
			return temperature_;
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