#ifndef OCTOON_CAUSTIC_FILM_CAMERA_H_
#define OCTOON_CAUSTIC_FILM_CAMERA_H_

#include <octoon/caustic/camera.h>

namespace octoon
{
	namespace caustic
	{
		class FilmCamera : public Camera
		{
		public:
			FilmCamera() noexcept;
			virtual ~FilmCamera() noexcept;

		private:
			FilmCamera(const FilmCamera&) noexcept = delete;
			FilmCamera& operator=(const FilmCamera&) noexcept = delete;
		};
	}
}

#endif