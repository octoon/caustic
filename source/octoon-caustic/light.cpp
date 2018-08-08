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
		Light::setColor(const float color[3]) noexcept
		{
			color_[0] = color[0];
			color_[1] = color[1];
			color_[2] = color[2];
		}

		void
		Light::getColor(float color[3]) const noexcept
		{
			color[0] = color_[0];
			color[1] = color_[1];
			color[2] = color_[2];
		}

		void 
		Light::sample(const float ro[3], const float norm[3], const Material& mat, const float Xi[2], float L[3]) const noexcept
		{
			L[0] = 0.0f;
			L[1] = 0.0f;
			L[2] = 0.0f;
		}
	}
}