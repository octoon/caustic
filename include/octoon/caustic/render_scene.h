#ifndef OCTOON_CAUSTIC_SCENE_H_
#define OCTOON_CAUSTIC_SCENE_H_

#include <octoon/caustic/camera.h>
#include <octoon/caustic/light.h>
#include <octoon/caustic/geometry.h>

namespace RadeonRays
{
	class IntersectionApi;
}

namespace octoon
{
	namespace caustic
	{
		class RenderScene final
		{
		public:
			typedef Camera* CameraPtr;
			typedef Light* LightPtr;
			typedef RenderObject* RenderObjectPtr;

		public:
			RenderScene() noexcept;
			~RenderScene() noexcept;

			static RenderScene& instance() noexcept;

			bool setup() noexcept;
			void close() noexcept;

			void addCamera(const CameraPtr& camera) noexcept;
			void removeCamera(const CameraPtr& camera) noexcept;
			const std::vector<CameraPtr>& getCameraList() const noexcept;

			void addLight(const LightPtr& object) noexcept;
			void removeLight(const LightPtr& object) noexcept;
			const std::vector<LightPtr>& getLightList() const noexcept;

			void addRenderObject(const RenderObjectPtr& object) noexcept;
			void removeRenderObject(const RenderObjectPtr& object) noexcept;
			const std::vector<RenderObjectPtr>& getRenderObjects() const noexcept;

		private:
			friend class Geometry;
			friend class RenderObject;
			RadeonRays::IntersectionApi* getIntersectionApi() const noexcept;

		private:
			RenderScene(const RenderScene&) = delete;
			RenderScene& operator=(const RenderScene&) = delete;

		private:
			RadeonRays::IntersectionApi* api_;

			std::vector<CameraPtr> cameras_;
			std::vector<LightPtr> lights_;
			std::vector<RenderObjectPtr> renderables_;
		};
	}
}

#endif