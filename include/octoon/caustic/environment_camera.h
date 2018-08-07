#ifndef OCTOON_CAUSTIC_ENVIRONMENT_CAMERA_H_
#define OCTOON_CAUSTIC_ENVIRONMENT_CAMERA_H_

#include <octoon/caustic/camera.h>

namespace octoon
{
	namespace caustic
	{
		class EnvironmentCamera final : public Camera
		{
		public:
			EnvironmentCamera() noexcept;
			virtual ~EnvironmentCamera() noexcept;

		private:
			EnvironmentCamera(const EnvironmentCamera&) noexcept = delete;
			EnvironmentCamera& operator=(const EnvironmentCamera&) noexcept = delete;
		};
	}
}

#endif