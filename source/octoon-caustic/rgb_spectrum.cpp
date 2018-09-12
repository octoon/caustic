#include <octoon/caustic/rgb_spectrum.h>

namespace octoon
{
	namespace caustic
	{
		RGBSpectrum::RGBSpectrum() noexcept
		{
			r = 0.0f;
			g = 0.0f;
			b = 0.0f;
		}

		RGBSpectrum::RGBSpectrum(float rr, float gg, float bb) noexcept
		{
			r = rr;
			g = gg;
			b = bb;
		}

		RGBSpectrum::RGBSpectrum(float gray) noexcept
		{
			r = gray;
			g = gray;
			b = gray;
		}

		RGBSpectrum::~RGBSpectrum() noexcept
		{
		}

		std::uint32_t 
		RGBSpectrum::getColor() const noexcept
		{
			std::uint32_t color = 0;

			color |= ((std::uint8_t)(255.0f*saturate(r))) << 16;
			color |= ((std::uint8_t)(255.0f*saturate(g))) << 8;
			color |= ((std::uint8_t)(255.0f*saturate(b))) << 0;

			return color;
		}

		void 
		RGBSpectrum::setColor(std::uint32_t color) noexcept
		{
			r = ((float)((color >> 16) & 255)) / 255.0f;
			g = ((float)((color >> 8) & 255)) / 255.0f;
			b = ((float)((color >> 0) & 255)) / 255.0f;
		}

		float 
		RGBSpectrum::getR() const  noexcept
		{
			return r;
		}

		float 
		RGBSpectrum::getG() const  noexcept
		{
			return g;
		}

		float 
		RGBSpectrum::getB() const  noexcept
		{
			return b;
		}

		float 
		RGBSpectrum::getMaxComponent() const noexcept
		{
			return std::max(r, std::max(b, g));
		}

		RGBSpectrum 
		RGBSpectrum::clamp(float low, float high) const noexcept
		{
			return RGBSpectrum(caustic::clamp(r, low, high), caustic::clamp(g, low, high), caustic::clamp(b, low, high));
		}

		RGBSpectrum 
		RGBSpectrum::operator + (const RGBSpectrum& c) const noexcept
		{
			float r_ = r + c.r;
			float g_ = g + c.g;
			float b_ = b + c.b;

			return RGBSpectrum(r_, g_, b_);
		}

		RGBSpectrum 
		RGBSpectrum::operator - (const RGBSpectrum& c) const noexcept
		{
			float r_ = r - c.r;
			float g_ = g - c.g;
			float b_ = b - c.b;

			return RGBSpectrum(r_, g_, b_);
		}

		RGBSpectrum 
		RGBSpectrum::operator * (const RGBSpectrum& c) const noexcept
		{
			float r_ = r * c.r;
			float g_ = g * c.g;
			float b_ = b * c.b;

			return RGBSpectrum(r_, g_, b_);
		}

		RGBSpectrum 
		RGBSpectrum::operator / (const RGBSpectrum& c) const noexcept
		{
			float r_ = r / c.r;
			float g_ = g / c.g;
			float b_ = b / c.b;

			return RGBSpectrum(r_, g_, b_);
		}

		RGBSpectrum 
		RGBSpectrum::operator + (float t) const noexcept
		{
			float r_ = t + r;
			float g_ = t + g;
			float b_ = t + b;

			return RGBSpectrum(r_, g_, b_);
		}

		RGBSpectrum 
		RGBSpectrum::operator - (float t) const noexcept
		{
			float r_ = r - t;
			float g_ = g - t;
			float b_ = b - t;

			return RGBSpectrum(r_, g_, b_);
		}

		RGBSpectrum 
		RGBSpectrum::operator * (float t) const noexcept
		{
			float r_ = t * r;
			float g_ = t * g;
			float b_ = t * b;

			return RGBSpectrum(r_, g_, b_);
		}

		RGBSpectrum 
		RGBSpectrum::operator / (float t) const noexcept
		{
			float r_ = r / t;
			float g_ = g / t;
			float b_ = b / t;

			return RGBSpectrum(r_, g_, b_);
		}
	}
}