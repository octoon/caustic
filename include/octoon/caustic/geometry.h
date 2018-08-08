#ifndef OCTOON_CAUSTIC_GEOMETRY_H_
#define OCTOON_CAUSTIC_GEOMETRY_H_

#include <octoon/caustic/object.h>
#include <octoon/caustic/material.h>

namespace octoon
{
	namespace caustic
	{
		class Geometry final : public Object
		{
		public:
			Geometry() noexcept;
			virtual ~Geometry() noexcept;

			void setMaterial(std::shared_ptr<Material>& material) noexcept;
			const std::shared_ptr<Material>& getMaterial() const noexcept;

		private:
			Geometry(const Geometry&) noexcept = delete;
			Geometry& operator=(const Geometry&) noexcept = delete;

		private:
			std::shared_ptr<Material> material_;
		};
	}
}

#endif