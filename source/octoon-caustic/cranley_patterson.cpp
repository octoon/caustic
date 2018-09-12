#include "cranley_patterson.h"
#include <octoon/caustic/math.h>

namespace octoon
{
	namespace caustic
	{
		CranleyPatterson::CranleyPatterson() noexcept
		{
		}

		CranleyPatterson::CranleyPatterson(std::unique_ptr<Sequences>&& seq) noexcept
			: sequences_(std::move(seq))
		{
		}

		CranleyPatterson::CranleyPatterson(std::unique_ptr<Sequences>&& seq, std::uint32_t size) noexcept
			: sequences_(std::move(seq))
		{
			this->init_random(size);
		}

		CranleyPatterson::~CranleyPatterson() noexcept
		{
		}

		void
		CranleyPatterson::setSequences(std::unique_ptr<Sequences>&& seq) noexcept
		{
			sequences_ = std::move(seq);
		}

		void
		CranleyPatterson::init_random(std::uint32_t size) noexcept
		{
			auto rand = [](float seed) { return fract(std::sin(seed) * 43758.5453123f); };

			random_.resize(size);

#pragma omp parallel for
			for (std::int32_t i = 0; i < size; ++i)
				random_[i] = rand(i - 64.340622f);
		}

		float
		CranleyPatterson::sample(std::uint32_t dimension, std::uint32_t frame) const noexcept
		{
			assert(sequences_);
			return sequences_->sample(dimension, frame);
		}

		float
		CranleyPatterson::sample(std::uint32_t dimension, std::uint32_t frame, std::uint32_t index) const noexcept
		{
			assert(sequences_);
			return fract(sequences_->sample(dimension, frame) + random_[index]);
		}
	}
}