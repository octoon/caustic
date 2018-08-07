#ifndef OCTOON_CAUSTIC_GEOMETRY_H_
#define OCTOON_CAUSTIC_GEOMETRY_H_

#include <octoon/caustic/object.h>

namespace octoon
{
	namespace caustic
	{
		class Geometry final : public Object
		{
		public:
			Geometry() noexcept;
			virtual ~Geometry() noexcept;

		private:
			Geometry(const Geometry&) noexcept;
			Geometry& operator=(const Geometry&) noexcept;
		};
	}
}

#endif