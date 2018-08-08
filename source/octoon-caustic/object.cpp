#include <octoon/caustic/object.h>
#include <memory>

namespace octoon
{
	namespace caustic
	{
		Object::Object() noexcept
		{
			transform_[0][0] = 1.0f; transform_[0][1] = 0.0f; transform_[0][2] = 0.0f; transform_[0][3] = 0.0f;
			transform_[1][0] = 0.0f; transform_[1][1] = 1.0f; transform_[1][2] = 0.0f; transform_[1][3] = 0.0f;
			transform_[2][0] = 0.0f; transform_[2][1] = 0.0f; transform_[2][2] = 1.0f; transform_[2][3] = 0.0f;
			transform_[3][0] = 0.0f; transform_[3][1] = 0.0f; transform_[3][2] = 0.0f; transform_[3][3] = 1.0f;

			transformInverse_[0][0] = 1.0f; transformInverse_[0][1] = 0.0f; transformInverse_[0][2] = 0.0f; transformInverse_[0][3] = 0.0f;
			transformInverse_[1][0] = 0.0f; transformInverse_[1][1] = 1.0f; transformInverse_[1][2] = 0.0f; transformInverse_[1][3] = 0.0f;
			transformInverse_[2][0] = 0.0f; transformInverse_[2][1] = 0.0f; transformInverse_[2][2] = 1.0f; transformInverse_[2][3] = 0.0f;
			transformInverse_[3][0] = 0.0f; transformInverse_[3][1] = 0.0f; transformInverse_[3][2] = 0.0f; transformInverse_[3][3] = 1.0f;
		}

		Object::~Object() noexcept
		{
		}

		void 
		Object::setTransform(const float m[4][4], const float minv[4][4]) noexcept
		{
			std::memcpy(transform_, m, sizeof(float) * 4 * 4);
			std::memcpy(transformInverse_, minv, sizeof(float) * 4 * 4);
		}

		const float*
		Object::getTransform() const noexcept
		{
			return (const float*)transform_;
		}

		const float*
		Object::getTransformInverse() const noexcept
		{
			return (const float*)transformInverse_;
		}

		void
		Object::getTranslate(float translate[3]) const noexcept
		{
			translate[0] = transform_[3][0];
			translate[1] = transform_[3][1];
			translate[2] = transform_[3][2];
		}
	}
}