#ifndef OCTOON_CAUSTIC_HALTON
#define OCTOON_CAUSTIC_HALTON

#include <memory>
#include <octoon/caustic/sequences.h>

class Halton_sampler;

namespace octoon
{
	namespace caustic
	{
		class Halton final : public Sequences
		{
		public:
			Halton() noexcept;
			~Halton() noexcept;

			float sample(std::uint32_t dimension, std::uint32_t index) const noexcept override;

		private:
			std::unique_ptr<Halton_sampler> haltonSampler_;
		};
	}
}

#endif