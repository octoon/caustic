#ifndef OCTOON_CAUSTIC_CRANLEY_PATTERSON
#define OCTOON_CAUSTIC_CRANLEY_PATTERSON

#include <vector>
#include <memory>

#include <octoon/caustic/sequences.h>

namespace octoon
{
	namespace caustic
	{
		class CranleyPatterson final : public Sequences
		{
		public:
			CranleyPatterson() noexcept;
			CranleyPatterson(std::unique_ptr<Sequences>&& seq) noexcept;
			~CranleyPatterson() noexcept;

			void setSequences(std::unique_ptr<Sequences>&& seq) noexcept;

			void init_random(std::uint32_t size) noexcept;

			float sample(std::uint32_t dimension, std::uint32_t index) const noexcept override;
			float sample(std::uint32_t dimension, std::uint32_t frame, std::uint32_t index) const noexcept;

		private:
			std::vector<float> random_;
			std::unique_ptr<Sequences> sequences_;
		};
	}
}

#endif