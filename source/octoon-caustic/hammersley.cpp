#include "hammersley.h"
#include "math.h"
#include <assert.h>

namespace octoon
{
	namespace caustic
	{
		inline std::uint32_t ReverseBits32(std::uint32_t bits)
		{
			bits = (bits << 16) | (bits >> 16);
			bits = ((bits & 0x00ff00ff) << 8) | ((bits & 0xff00ff00) >> 8);
			bits = ((bits & 0x0f0f0f0f) << 4) | ((bits & 0xf0f0f0f0) >> 4);
			bits = ((bits & 0x33333333) << 2) | ((bits & 0xcccccccc) >> 2);
			bits = ((bits & 0x55555555) << 1) | ((bits & 0xaaaaaaaa) >> 1);
			return bits;
		}

		Hammersley::Hammersley() noexcept
			: maxSamples_(1024)
		{
		}

		Hammersley::Hammersley(std::uint32_t maxSamples) noexcept
			: maxSamples_(maxSamples)
		{
		}

		Hammersley::~Hammersley() noexcept
		{
		}

		float 
		Hammersley::sample(std::uint32_t dimension, std::uint32_t index) const noexcept
		{
			switch (dimension)
			{
			case 0:
				return float(index) / maxSamples_;
			case 1:
				return ReverseBits32(index) * 2.3283064365386963e-10f;
			default:
				assert(false);
				return 0;
			}
		}

		float 
		Hammersley::sample(std::uint32_t dimension, std::uint32_t index, std::uint32_t seed) const noexcept
		{
			switch (dimension)
			{
			case 0:
				return fract(float(index) / maxSamples_ + float(seed & 0xffff) / (1 << 16));
			case 1:
				return ReverseBits32(index ^ seed) * 2.3283064365386963e-10f;
			break;
			default:
				assert(false);
				return 0;
			}
		}
	}
}