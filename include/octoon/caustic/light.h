#ifndef OCTOON_CAUSTIC_LIGHT_H_
#define OCTOON_CAUSTIC_LIGHT_H_

#include <octoon/caustic/object.h>

namespace octoon
{
	namespace caustic
	{
		class Light : public Object
		{
		public:
			Light() noexcept;
			virtual ~Light() noexcept;

		private:
			Light(const Light&) noexcept;
			Light& operator=(const Light&) noexcept;
		};
	}
}

#endif