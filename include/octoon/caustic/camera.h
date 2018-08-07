#ifndef OCTOON_CAUSTIC_CAMERA_H_
#define OCTOON_CAUSTIC_CAMERA_H_

#include <octoon/caustic/object.h>

namespace octoon
{
	namespace caustic
	{
		class Camera : public Object
		{
		public:
			Camera() noexcept;
			virtual ~Camera() noexcept;

		private:
			Camera(const Camera&) noexcept;
			Camera& operator=(const Camera&) noexcept;
		};
	}
}

#endif