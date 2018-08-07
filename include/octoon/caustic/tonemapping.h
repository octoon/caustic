#ifndef OCTOON_CAUSTIC_TONEMAPPING_H_
#define OCTOON_CAUSTIC_TONEMAPPING_H_

#include <algorithm>

namespace octoon
{
	namespace caustic
	{
		class Tonemapping
		{
		public:
			Tonemapping() noexcept;
			virtual ~Tonemapping() noexcept;

			virtual float map(float x) noexcept = 0;

		private:
			Tonemapping(const Tonemapping&) noexcept;
			Tonemapping& operator=(const Tonemapping&) noexcept;
		};
	}
}

#endif