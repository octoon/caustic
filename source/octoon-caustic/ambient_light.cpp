#include <octoon/caustic/ambient_light.h>

namespace octoon
{
	namespace caustic
	{
		AmbientLight::AmbientLight() noexcept
		{
		}

		AmbientLight::AmbientLight(const float color[3]) noexcept
		{
			this->setColor(color);
		}

		AmbientLight::~AmbientLight() noexcept
		{
		}

		void 
		AmbientLight::sample(const float ro[3], const float norm[3], const Material& mat, const float Xi[2], float L[3]) const noexcept
		{
			L[0] = 0.0f;
			L[1] = 0.0f;
			L[2] = 0.0f;
		}
	}
}