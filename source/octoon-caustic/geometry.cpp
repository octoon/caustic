#include <octoon/caustic/geometry.h>
#include <octoon/caustic/render_scene.h>

namespace octoon
{
	namespace caustic
	{
		Geometry::Geometry() noexcept
			: mesh_(nullptr)
		{
		}

		Geometry::~Geometry() noexcept
		{
		}

		void 
		Geometry::setMaterial(std::shared_ptr<Material>& material) noexcept
		{
			material_ = material;
		}

		const std::shared_ptr<Material>&
		Geometry::getMaterial() const noexcept
		{
			return material_;
		}

		void
		Geometry::setShape(const float* vertices, int vnum, int vstride, const int* indices, int istride, const int* numfacevertices, int numfaces) noexcept
		{
			auto api = RenderScene::instance().getIntersectionApi();
			if (mesh_)
				api->DeleteShape(mesh_);

			mesh_ = api->CreateMesh(vertices, vnum, vstride, indices, istride, numfacevertices, numfaces);

			api->Commit();
		}
	}
}