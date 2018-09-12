#ifndef	OCTOON_CAUSTIC_SPECTRUM
#define	OCTOON_CAUSTIC_SPECTRUM

#include <octoon/caustic/rgb_spectrum.h>

namespace octoon
{
	namespace caustic
	{
		class Spectrum final : public RGBSpectrum
		{
		public:
			Spectrum() noexcept;
			~Spectrum() noexcept;

			template<typename ...Args>
			Spectrum(Args&&... args)
				: RGBSpectrum(std::forward<Args>(args)...)
			{
			}
		};
	}
}

#endif