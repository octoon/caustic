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

			void setTransform(const float m[4][4], const float minv[4][4]) noexcept;
			void getTransform(const float m[4][4], const float minv[4][4]) const noexcept;

		private:
			Geometry(const Geometry&) noexcept = delete;
			Geometry& operator=(const Geometry&) noexcept = delete;
		};
	}
}

#endif