#ifndef OCTOON_CRANLEY_PATTERSON
#define OCTOON_CRANLEY_PATTERSON

#include "sequences.h"
#include <vector>

namespace octoon
{
	class CranleyPatterson final : public Sequences
	{
	public:
		CranleyPatterson() noexcept;
		CranleyPatterson(std::unique_ptr<Sequences>&& seq) noexcept;
		~CranleyPatterson() noexcept;

		void setSequences(std::unique_ptr<Sequences>&& seq) noexcept;

		void init_random(std::uint32_t size) noexcept;

		float sample(std::uint32_t dimension, std::uint32_t index) const noexcept;
		float sample(std::uint32_t dimension, std::uint32_t frame, std::uint32_t index) const noexcept;

	private:
		std::vector<float> random_;
		std::unique_ptr<Sequences> sequences_;
	};
}

#endif