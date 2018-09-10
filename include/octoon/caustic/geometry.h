#ifndef OCTOON_CAUSTIC_GEOMETRY_H_
#define OCTOON_CAUSTIC_GEOMETRY_H_

#include <octoon/caustic/render_object.h>
#include <octoon/caustic/material.h>

namespace RadeonRays
{
	class Shape;
}

namespace octoon
{
	namespace caustic
	{
		class Geometry final : public RenderObject
		{
		public:
			Geometry() noexcept;
			virtual ~Geometry() noexcept;

			void setMaterial(std::shared_ptr<Material>& material) noexcept;
			const std::shared_ptr<Material>& getMaterial() const noexcept;

			void setShape(const float* vertices, int vnum, int vstride, const int* indices, int istride, const int* numfacevertices, int numfaces) noexcept;

		private:
			Geometry(const Geometry&) noexcept = delete;
			Geometry& operator=(const Geometry&) noexcept = delete;

		private:
			std::shared_ptr<Material> material_;
			RadeonRays::Shape* mesh_;
		};
	}
}

#endif