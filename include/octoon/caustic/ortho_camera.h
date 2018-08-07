#ifndef OCTOON_CAUSTIC_ORTHO_CAMERA_H_
#define OCTOON_CAUSTIC_ORTHO_CAMERA_H_

#include <octoon/caustic/camera.h>

namespace octoon
{
	namespace caustic
	{
		class OrthoCamera final : public Camera
		{
		public:
			OrthoCamera() noexcept;
			virtual ~OrthoCamera() noexcept;

		private:
			OrthoCamera(const OrthoCamera&) noexcept;
			OrthoCamera& operator=(const OrthoCamera&) noexcept;
		};
	}
}

#endif