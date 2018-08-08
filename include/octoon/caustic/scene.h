#ifndef OCTOON_CAUSTIC_SCENE_H_
#define OCTOON_CAUSTIC_SCENE_H_

#include <vector>
#include <octoon/caustic/object.h>

namespace octoon
{
	namespace caustic
	{
		class Scene final
		{
		public:
			Scene() noexcept;
			~Scene() noexcept;

			void addCamera(class Camera* camera) noexcept;
			void removeCamera(Camera* camera) noexcept;
			const std::vector<Camera*>& getCameraList() const noexcept;

			void addLight(class Light* object) noexcept;
			void removeLight(Light* object) noexcept;
			const std::vector<Light*>& getLightList() const noexcept;

			void addRenderObject(Object* object) noexcept;
			void removeRenderObject(Object* object) noexcept;
			const std::vector<Object*>& getRenderObjects() const noexcept;

		private:
			Scene(const Scene&) = delete;
			Scene& operator=(const Scene&) = delete;

		private:
			std::vector<Camera*> cameras_;
			std::vector<Light*> lights_;
			std::vector<Object*> renderables_;
		};
	}
}

#endif