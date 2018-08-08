#include <octoon/caustic/geometry.h>
#include <memory>

namespace octoon
{
	namespace caustic
	{
		Geometry::Geometry() noexcept
		{
		}

		Geometry::~Geometry() noexcept
		{
		}

		void 
		Geometry::setMaterial(std::shared_ptr<Material>& material) noexcept
		{
		}

		const std::shared_ptr<Material>&
		Geometry::getMaterial() const noexcept
		{
			return material_;
		}
	}
}