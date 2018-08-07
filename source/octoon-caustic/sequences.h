#ifndef OCTOON_CAUSTIC_SEQUENCES
#define OCTOON_CAUSTIC_SEQUENCES

#include <algorithm>
#include <memory>

namespace octoon
{
	class Sequences
	{
	public:
		Sequences() noexcept;
		virtual ~Sequences() noexcept;

		virtual float sample(std::uint32_t dimension, std::uint32_t index) const noexcept = 0;

	private:
		Sequences(const Sequences&) = delete;
		Sequences& operator=(const Sequences&) = delete;
	};
}

#endif