#include <octoon/caustic/object.h>
#include <memory>

namespace octoon
{
	namespace caustic
	{
		Object::Object() noexcept
		{
		}

		Object::~Object() noexcept
		{
		}

		void
		Object::setTransform(const RadeonRays::matrix& m, const RadeonRays::matrix& minv) noexcept
		{
			transform_ = m;
			transformInverse_ = minv;
		}

		const RadeonRays::matrix&
		Object::getTransform() const noexcept
		{
			return transform_;
		}

		const RadeonRays::matrix&
		Object::getTransformInverse() const noexcept
		{
			return transformInverse_;
		}

		RadeonRays::float3
		Object::getTranslate() const noexcept
		{
			return RadeonRays::float3(transform_.m30, transform_.m31, transform_.m32);
		}
	}
}