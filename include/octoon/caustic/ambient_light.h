#ifndef OCTOON_CAUSTIC_AMBIENT_LIGHT_H_
#define OCTOON_CAUSTIC_AMBIENT_LIGHT_H_

#include <octoon/caustic/light.h>

namespace octoon
{
	namespace caustic
	{
		class AmbientLight : public Light
		{
		public:
			AmbientLight() noexcept;
			AmbientLight(const float color[3]) noexcept;
			virtual ~AmbientLight() noexcept;

			virtual void sample(const float ro[3], const float norm[3], const Material& mat, const float Xi[2], float L[3]) const noexcept override;

		private:
			AmbientLight(const AmbientLight&) noexcept = delete;
			AmbientLight& operator=(const AmbientLight&) noexcept = delete;
		};
	}
}

#endif