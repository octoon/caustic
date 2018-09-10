#include <octoon/caustic/render_object.h>
#include <octoon/caustic/render_scene.h>

namespace octoon
{
	namespace caustic
	{
		RenderObject::RenderObject() noexcept
			: active_(false)
			, visible_(true)
			, layer_(0)
		{
		}

		RenderObject::~RenderObject() noexcept
		{
		}

		void
		RenderObject::setActive(bool active) noexcept
		{
			if (active_ != active)
			{
				if (active)
					this->onActivate();
				else
					this->onDeactivate();

				active_ = active;
			}
		}

		bool
		RenderObject::getActive() const noexcept
		{
			return active_;
		}

		void
		RenderObject::setLayer(std::uint8_t layer) noexcept
		{
			layer_ = layer;
		}

		std::uint8_t
		RenderObject::getLayer() const noexcept
		{
			return layer_;
		}

		void
		RenderObject::setVisible(bool enable) noexcept
		{
			visible_ = enable;
		}

		bool
		RenderObject::getVisible() const noexcept
		{
			return visible_;
		}

		void
		RenderObject::setTransform(const RadeonRays::matrix& m, const RadeonRays::matrix& minv) noexcept
		{
			transform_ = m;
			transformInverse_ = minv;
		}

		const RadeonRays::matrix&
		RenderObject::getTransform() const noexcept
		{
			return transform_;
		}

		const RadeonRays::matrix&
		RenderObject::getTransformInverse() const noexcept
		{
			return transformInverse_;
		}

		RadeonRays::float3
		RenderObject::getTranslate() const noexcept
		{
			return RadeonRays::float3(transform_.m30, transform_.m31, transform_.m32);
		}

		void
		RenderObject::onActivate() noexcept
		{
			RenderScene::instance().addRenderObject(this);
		}

		void
		RenderObject::onDeactivate() noexcept
		{
			RenderScene::instance().removeRenderObject(this);
		}
	}
}