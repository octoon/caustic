#ifndef OCTOON_CAUSTIC_OBJECT_H_
#define OCTOON_CAUSTIC_OBJECT_H_

#include <memory>
#include <assert.h>
#include <radeon_rays.h>

namespace octoon
{
	namespace caustic
	{
		class RenderObject : public std::enable_shared_from_this<RenderObject>
		{
		public:
			RenderObject() noexcept;
			virtual ~RenderObject() noexcept;

			void setActive(bool active) noexcept;
			bool getActive() const noexcept;

			void setLayer(std::uint8_t layer) noexcept;
			std::uint8_t getLayer() const noexcept;

			void setVisible(bool enable) noexcept;
			bool getVisible() const noexcept;

			void setTransform(const RadeonRays::matrix& m, const RadeonRays::matrix& minv) noexcept;
			const RadeonRays::matrix& getTransform() const noexcept;
			const RadeonRays::matrix& getTransformInverse() const noexcept;

			RadeonRays::float3 getTranslate() const noexcept;

		public:
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
			virtual void onActivate() noexcept;
			virtual void onDeactivate() noexcept;

		private:
			RenderObject(const RenderObject&) = delete;
			RenderObject& operator=(const RenderObject&) = delete;

		private:
			bool active_;
			bool visible_;

			std::uint8_t layer_;

			RadeonRays::matrix transform_;
			RadeonRays::matrix transformInverse_;
		};
	}
}

#endif