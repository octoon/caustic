#ifndef OCTOON_CAUSTIC_HALTON
#define OCTOON_CAUSTIC_HALTON

#include "sequences.h"

class Halton_sampler;

namespace octoon
{
	class Halton final : public Sequences
	{
	public:
		Halton() noexcept;
		~Halton() noexcept;

		float sample(std::uint32_t dimension, std::uint32_t index) const noexcept;

	private:
		std::unique_ptr<Halton_sampler> haltonSampler_;
	};
}

#endif