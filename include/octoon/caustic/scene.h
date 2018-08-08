#ifndef OCTOON_CAUSTIC_SCENE_H_
#define OCTOON_CAUSTIC_SCENE_H_

#include <vector>
#include <memory>
#include <octoon/caustic/object.h>

namespace octoon
{
	namespace caustic
	{
		class Scene final
		{
		public:
			typedef std::shared_ptr<class Camera> CameraPtr;
			typedef std::shared_ptr<class Light> LightPtr;
			typedef std::shared_ptr<class Object> ObjectPtr;

		public:
			Scene() noexcept;
			~Scene() noexcept;

			void addCamera(const CameraPtr& camera) noexcept;
			void removeCamera(const CameraPtr& camera) noexcept;
			const std::vector<CameraPtr>& getCameraList() const noexcept;

			void addLight(const LightPtr& object) noexcept;
			void removeLight(const LightPtr& object) noexcept;
			const std::vector<LightPtr>& getLightList() const noexcept;

			void addRenderObject(const ObjectPtr& object) noexcept;
			void removeRenderObject(const ObjectPtr& object) noexcept;
			const std::vector<ObjectPtr>& getRenderObjects() const noexcept;

		private:
			Scene(const Scene&) = delete;
			Scene& operator=(const Scene&) = delete;

		private:
			std::vector<CameraPtr> cameras_;
			std::vector<LightPtr> lights_;
			std::vector<ObjectPtr> renderables_;
		};
	}
}

#endif