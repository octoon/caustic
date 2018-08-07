#include <octoon/caustic/scene.h>
#include <octoon/caustic/camera.h>
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
		Scene::addCamera(Camera* camera) noexcept
		{
			assert(camera);

			auto it = std::find(cameras_.begin(), cameras_.end(), camera);
			if (it == cameras_.end())
			{
				cameras_.push_back(camera);
			}
		}

		void
		Scene::removeCamera(Camera* camera) noexcept
		{
			assert(camera);

			auto it = std::find(cameras_.begin(), cameras_.end(), camera);
			if (it != cameras_.end())
				cameras_.erase(it);
		}

		const std::vector<Camera*>&
		Scene::getCameraList() const noexcept
		{
			return cameras_;
		}

		void
		Scene::addRenderObject(Object* object) noexcept
		{
			assert(object);

			if (object->isA<Camera>())
				this->addCamera(object->downcast<Camera>());
			else
				renderables_.push_back(object);
		}

		void
		Scene::removeRenderObject(Object* object) noexcept
		{
			assert(object);

			if (object->isA<Camera>())
			{
				auto it = std::find(cameras_.begin(), cameras_.end(), object->downcast<Camera>());
				if (it != cameras_.end())
					cameras_.erase(it);
			}
			else
			{
				auto it = std::find(renderables_.begin(), renderables_.end(), object);
				if (it != renderables_.end())
					renderables_.erase(it);
			}
		}

		const std::vector<Object*>&
		Scene::getRenderObjects() const noexcept
		{
			return renderables_;
		}
	}
}