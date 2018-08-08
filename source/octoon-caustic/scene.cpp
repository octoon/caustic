#include <octoon/caustic/scene.h>
#include <octoon/caustic/camera.h>
#include <octoon/caustic/light.h>
#include <cassert>

namespace octoon
{
	namespace caustic
	{
		Scene::Scene() noexcept
		{
		}

		Scene::~Scene() noexcept
		{
		}

		void
		Scene::addCamera(const CameraPtr& camera) noexcept
		{
			assert(camera);

			auto it = std::find(cameras_.begin(), cameras_.end(), camera);
			if (it == cameras_.end())
			{
				cameras_.push_back(camera);
			}
		}

		void
		Scene::removeCamera(const CameraPtr& camera) noexcept
		{
			assert(camera);

			auto it = std::find(cameras_.begin(), cameras_.end(), camera);
			if (it != cameras_.end())
				cameras_.erase(it);
		}

		const std::vector<Scene::CameraPtr>&
		Scene::getCameraList() const noexcept
		{
			return cameras_;
		}

		void
		Scene::addLight(const LightPtr& light) noexcept
		{
			assert(light);

			auto it = std::find(lights_.begin(), lights_.end(), light);
			if (it == lights_.end())
			{
				lights_.push_back(light);
			}
		}

		void
		Scene::removeLight(const LightPtr& light) noexcept
		{
			assert(light);

			auto it = std::find(lights_.begin(), lights_.end(), light);
			if (it != lights_.end())
				lights_.erase(it);
		}

		const std::vector<Scene::LightPtr>&
		Scene::getLightList() const noexcept
		{
			return lights_;
		}

		void
		Scene::addRenderObject(const ObjectPtr& object) noexcept
		{
			assert(object);

			if (object->isA<Camera>())
				this->addCamera(object->downcast_pointer<Camera>());
			else if (object->isA<Light>())
				this->addLight(object->downcast_pointer<Light>());
			else
				renderables_.push_back(object);
		}

		void
		Scene::removeRenderObject(const ObjectPtr& object) noexcept
		{
			assert(object);

			if (object->isA<Camera>())
			{
				auto it = std::find(cameras_.begin(), cameras_.end(), object->downcast_pointer<Camera>());
				if (it != cameras_.end())
					cameras_.erase(it);
			}
			else if (object->isA<Light>())
			{
				auto it = std::find(lights_.begin(), lights_.end(), object->downcast_pointer<Light>());
				if (it != lights_.end())
					lights_.erase(it);
			}
			else
			{
				auto it = std::find(renderables_.begin(), renderables_.end(), object);
				if (it != renderables_.end())
					renderables_.erase(it);
			}
		}

		const std::vector<Scene::ObjectPtr>&
		Scene::getRenderObjects() const noexcept
		{
			return renderables_;
		}
	}
}