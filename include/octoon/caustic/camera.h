#ifndef OCTOON_CAUSTIC_CAMERA_H_
#define OCTOON_CAUSTIC_CAMERA_H_

namespace octoon
{
	namespace caustic
	{
		class Camera
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