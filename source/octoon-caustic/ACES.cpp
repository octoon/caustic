#include <octoon/caustic/ACES.h>

namespace octoon
{
	namespace caustic
	{
		ACES::ACES() noexcept
		{
		}

		ACES::~ACES() noexcept
		{
		}

		float
		ACES::map(float x) noexcept
		{
			const float A = 2.51f;
			const float B = 0.03f;
			const float C = 2.43f;
			const float D = 0.59f;
			const float E = 0.14f;
			return std::pow(std::min(1.0f, (x * (A * x + B)) / (x * (C * x + D) + E)), 1.0f / 2.2f);
		}
	}
}