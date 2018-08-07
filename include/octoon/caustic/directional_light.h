#ifndef OCTOON_CAUSTIC_DIRECTIONAL_LIGHT_H_
#define OCTOON_CAUSTIC_DIRECTIONAL_LIGHT_H_

#include <octoon/caustic/light.h>

namespace octoon
{
	namespace caustic
	{
		class DirectionalLight : public Light
		{
		public:
			DirectionalLight() noexcept;
			virtual ~DirectionalLight() noexcept;

		private:
			DirectionalLight(const DirectionalLight&) noexcept;
			DirectionalLight& operator=(const DirectionalLight&) noexcept;
		};
	}
}

#endif