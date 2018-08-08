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
	}
}