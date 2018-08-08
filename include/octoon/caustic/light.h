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

			void setColor(const float color[3]) noexcept;
			void getColor(float color[3]) const noexcept;

		private:
			Light(const Light&) noexcept = delete;
			Light& operator=(const Light&) noexcept = delete;

		private:
			float color_[3];
		};
	}
}

#endif