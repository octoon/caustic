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
			std::memcpy(transform_, m, sizeof(m));
			std::memcpy(transformInverse_, minv, sizeof(minv));
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
	}
}