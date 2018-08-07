#include "halton.h"
#include "halton_enum.h"
#include "halton_sampler.h"

#include <assert.h>

namespace octoon
{
	Halton::Halton() noexcept
		: haltonSampler_(std::make_unique<Halton_sampler>())
	{
		haltonSampler_->init_faure();
	}

	Halton::~Halton() noexcept
	{
	}

	float 
	Halton::sample(std::uint32_t dimension, std::uint32_t index) const noexcept
	{
		return haltonSampler_->sample(dimension, index);
	}
}