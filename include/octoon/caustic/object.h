#ifndef OCTOON_CAUSTIC_OBJECT_H_
#define OCTOON_CAUSTIC_OBJECT_H_

namespace octoon
{
	namespace caustic
	{
		class Object
		{
		public:
			Object() noexcept;
			virtual ~Object() noexcept;

			void setTransform(const float m[4][4], const float minv[4][4]) noexcept;
			const float* getTransform() const noexcept;
			const float* getTransformInverse() const noexcept;

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