#ifndef OCTOON_CAUSTIC_LIGHT_H_
#define OCTOON_CAUSTIC_LIGHT_H_

#include <octoon/caustic/object.h>
#include <octoon/caustic/material.h>

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

			virtual void sample(const float ro[3], const float norm[3], const Material& mat, const float Xi[2], float L[3]) const noexcept;

		private:
			Light(const Light&) noexcept = delete;
			Light& operator=(const Light&) noexcept = delete;

		private:
			float color_[3];
		};
	}
}

#endif