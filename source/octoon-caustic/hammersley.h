#ifndef OCTOON_CAUSTIC_HAMMERSLEY
#define OCTOON_CAUSTIC_HAMMERSLEY

#include "sequences.h"

namespace octoon
{
	class Hammersley final : public Sequences
	{
	public:
		Hammersley() noexcept;
		Hammersley(std::uint32_t maxSamples) noexcept;
		~Hammersley() noexcept;

		float sample(std::uint32_t dimension, std::uint32_t index) const noexcept;
		float sample(std::uint32_t dimension, std::uint32_t index, std::uint32_t seed) const noexcept;

	private:
		std::uint32_t maxSamples_;
	};
}

#endif