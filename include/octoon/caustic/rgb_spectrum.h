#ifndef	OCTOON_CAUSTIC_RGBSPECTRUM
#define	OCTOON_CAUSTIC_RGBSPECTRUM

#include <octoon/caustic/math.h>

namespace octoon
{
	namespace caustic
	{
		class RGBSpectrum
		{
		public:
			RGBSpectrum() noexcept;
			RGBSpectrum(float gray) noexcept;
			RGBSpectrum(float r, float g, float b) noexcept;
			~RGBSpectrum() noexcept;

			std::uint32_t getColor() const noexcept;

			void setColor(std::uint32_t color) noexcept;
			void setColor(float r_, float g_, float b_) noexcept
			{
				r = r_; g = g_; b = b_;
			}

			float getR() const noexcept;
			float getG() const noexcept;
			float getB() const noexcept;
			float getMaxComponent() const noexcept;

			RGBSpectrum clamp(float low = 0.0f, float high = 0.0f) const noexcept;

			RGBSpectrum operator+(const RGBSpectrum& c) const noexcept;
			RGBSpectrum operator-(const RGBSpectrum& c) const noexcept;
			RGBSpectrum operator*(const RGBSpectrum& c) const noexcept;
			RGBSpectrum operator/(const RGBSpectrum& c) const noexcept;

			RGBSpectrum operator+(float t) const noexcept;
			RGBSpectrum operator-(float t) const noexcept;
			RGBSpectrum operator*(float t) const noexcept;
			RGBSpectrum operator/(float t) const noexcept;

			RGBSpectrum& operator+= (const RGBSpectrum& c) noexcept { *this = *this + c; return *this; }
			RGBSpectrum& operator-= (const RGBSpectrum& c) noexcept { *this = *this - c; return *this; }
			RGBSpectrum& operator*= (const RGBSpectrum& c) noexcept { *this = *this * c; return *this; }
			RGBSpectrum& operator/= (const RGBSpectrum& c) noexcept { *this = *this / c; return *this; }

			RGBSpectrum& operator+=(float t) noexcept { *this = *this + t; return *this; }
			RGBSpectrum& operator-=(float t) noexcept { *this = *this - t; return *this; }
			RGBSpectrum& operator*=(float t) noexcept { *this = *this * t; return *this; }
			RGBSpectrum& operator/=(float t) noexcept { *this = *this / t; return *this; }

			bool IsBlack() const noexcept
			{
				if (r > 0.0f)
					return false;
				if (g > 0.0f)
					return false;
				if (b > 0.0f)
					return false;
				return true;
			}

			float getIntensity() const noexcept
			{
				static const float YWeight[3] = { 0.212671f, 0.715160f, 0.072169f };
				return YWeight[0] * r + YWeight[1] * g + YWeight[2] * b;
			}

			RGBSpectrum exp() const noexcept
			{
				return RGBSpectrum((float)::exp(r), (float)::exp(g), (float)::exp(b));
			}

			friend RGBSpectrum operator+(float t, const RGBSpectrum& s) noexcept
			{
				return s + t;
			}

			friend RGBSpectrum operator-(float t, const RGBSpectrum& s) noexcept
			{
				float r = t - s.r;
				float g = t - s.g;
				float b = t - s.b;

				return RGBSpectrum(r, g, b);
			}

			friend RGBSpectrum operator*(float t, const RGBSpectrum& s) noexcept
			{
				return s * t;
			}

		private:
			float r;
			float g;
			float b;
		};
	}
}

#endif