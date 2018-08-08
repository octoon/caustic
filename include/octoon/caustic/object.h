#ifndef OCTOON_CAUSTIC_OBJECT_H_
#define OCTOON_CAUSTIC_OBJECT_H_

#include <memory>
#include <assert.h>
#include <radeon_rays.h>

namespace octoon
{
	namespace caustic
	{
		class Object : public std::enable_shared_from_this<Object>
		{
		public:
			Object() noexcept;
			virtual ~Object() noexcept;

			void setTransform(const float m[4][4], const float minv[4][4]) noexcept;
			const float* getTransform() const noexcept;
			const float* getTransformInverse() const noexcept;

			RadeonRays::float3 getTranslate() const noexcept;

			template<typename T>
			bool isA()
			{
				return dynamic_cast<T*>(this) != nullptr;
			}

			template<typename T>
			T* downcast()
			{
				return dynamic_cast<T*>(this);
			}

			template<typename T>
			std::shared_ptr<T> downcast_pointer() noexcept
			{
				assert(this->isA<T>());
				return std::dynamic_pointer_cast<T>(this->shared_from_this());
			}

		private:
			Object(const Object&) = delete;
			Object& operator=(const Object&) = delete;

		private:
			float transform_[4][4];
			float transformInverse_[4][4];
		};
	}
}

#endif