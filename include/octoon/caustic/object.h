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
		};
	}
}

#endif