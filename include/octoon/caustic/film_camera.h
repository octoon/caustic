#ifndef OCTOON_CAUSTIC_FILM_CAMERA_H_
#define OCTOON_CAUSTIC_FILM_CAMERA_H_

namespace octoon
{
	namespace caustic
	{
		class FilmCamera
		{
		public:
			FilmCamera() noexcept;
			virtual ~FilmCamera() noexcept;

		private:
			FilmCamera(const FilmCamera&) noexcept;
			FilmCamera& operator=(const FilmCamera&) noexcept;
		};
	}
}

#endif