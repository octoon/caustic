#include <octoon/caustic/render_scene.h>
#include <cassert>

namespace octoon
{
	namespace caustic
	{
		RenderScene::RenderScene() noexcept
			: api_(nullptr)
		{
		}

		RenderScene::~RenderScene() noexcept
		{
			this->close();
		}

		RenderScene&
		RenderScene::instance() noexcept
		{
			static RenderScene scene;
			return scene;
		}

		bool
		RenderScene::setup() noexcept
		{
			assert(this->api_ == nullptr);

			RadeonRays::IntersectionApi::SetPlatform(RadeonRays::DeviceInfo::kAny);

			auto deviceidx = std::string::npos;
			for (auto i = 0U; i < RadeonRays::IntersectionApi::GetDeviceCount(); ++i)
			{
				RadeonRays::DeviceInfo devinfo;
				RadeonRays::IntersectionApi::GetDeviceInfo(i, devinfo);

				if (devinfo.type == RadeonRays::DeviceInfo::kGpu)
				{
					std::string info_name(devinfo.name);
					if (info_name.find("Intel") != std::string::npos)
						continue;
					deviceidx = i;
					break;
				}
			}

			if (deviceidx == std::string::npos)
			{
				for (auto i = 0U; i < RadeonRays::IntersectionApi::GetDeviceCount(); ++i)
				{
					RadeonRays::DeviceInfo devinfo;
					RadeonRays::IntersectionApi::GetDeviceInfo(i, devinfo);

					if (devinfo.type == RadeonRays::DeviceInfo::kCpu)
					{
						deviceidx = i;
						break;
					}
				}
			}

			if (deviceidx == std::string::npos) return false;

			this->api_ = RadeonRays::IntersectionApi::Create(deviceidx);
			return this->api_ != nullptr;
		}

		void 
		RenderScene::close() noexcept
		{
			if (this->api_)
			{
				this->api_->DetachAll();
				RadeonRays::IntersectionApi::Delete(this->api_);
			}
		}

		void
		RenderScene::addCamera(const CameraPtr& camera) noexcept
		{
			assert(camera);

			auto it = std::find(cameras_.begin(), cameras_.end(), camera);
			if (it == cameras_.end())
				cameras_.push_back(camera);
		}

		void
		RenderScene::removeCamera(const CameraPtr& camera) noexcept
		{
			assert(camera);

			auto it = std::find(cameras_.begin(), cameras_.end(), camera);
			if (it != cameras_.end())
				cameras_.erase(it);
		}

		const std::vector<RenderScene::CameraPtr>&
		RenderScene::getCameraList() const noexcept
		{
			return cameras_;
		}

		void
		RenderScene::addLight(const LightPtr& light) noexcept
		{
			assert(light);

			auto it = std::find(lights_.begin(), lights_.end(), light);
			if (it == lights_.end())
				lights_.push_back(light);
		}

		void
		RenderScene::removeLight(const LightPtr& light) noexcept
		{
			assert(light);

			auto it = std::find(lights_.begin(), lights_.end(), light);
			if (it != lights_.end())
				lights_.erase(it);
		}

		const std::vector<RenderScene::LightPtr>&
		RenderScene::getLightList() const noexcept
		{
			return lights_;
		}

		void
		RenderScene::addRenderObject(const RenderObjectPtr& object) noexcept
		{
			assert(object);

			if (object->isA<Camera>())
				this->addCamera(object->downcast<Camera>());
			else if (object->isA<Light>())
				this->addLight(object->downcast<Light>());
			else
				renderables_.push_back(object);
		}

		void
		RenderScene::removeRenderObject(const RenderObjectPtr& object) noexcept
		{
			assert(object);

			if (object->isA<Camera>())
			{
				auto it = std::find(cameras_.begin(), cameras_.end(), object->downcast<Camera>());
				if (it != cameras_.end())
					cameras_.erase(it);
			}
			else if (object->isA<Light>())
			{
				auto it = std::find(lights_.begin(), lights_.end(), object->downcast<Light>());
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

		const std::vector<RenderScene::RenderObjectPtr>&
		RenderScene::getRenderObjects() const noexcept
		{
			return renderables_;
		}

		RadeonRays::IntersectionApi* 
		RenderScene::getIntersectionApi() const noexcept
		{
			return this->api_;
		}
	}
}