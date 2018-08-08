#ifndef OCTOON_CAUSTIC_POINT_LIGHT_H_
#define OCTOON_CAUSTIC_POINT_LIGHT_H_

#include <octoon/caustic/light.h>

namespace octoon
{
	namespace caustic
	{
		class PointLight : public Light
		{
		public:
			PointLight() noexcept;
			virtual ~PointLight() noexcept;

			virtual void sample(const float ro[3], const float norm[3], const Material& mat, const float Xi[2], float L[3]) const noexcept override;

		private:
			PointLight(const PointLight&) noexcept = delete;
			PointLight& operator=(const PointLight&) noexcept = delete;
		};
	}
}

#endif