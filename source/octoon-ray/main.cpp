#include <radeon_rays.h>
#include <assert.h>
#include <fstream>
#include "tiny_obj_loader.h"

struct Scene
{
	std::vector<tinyobj::shape_t> g_objshapes;
	std::vector<tinyobj::material_t> g_objmaterials;

	unsigned g_vertex_buffer, g_index_buffer;
	unsigned g_texture;
	
	int g_window_width = 640;
	int g_window_height = 480;

	// Point light position
	RadeonRays::float3 light = { -0.01f, 1.9f, 0.1f };

	RadeonRays::IntersectionApi* api;
	RadeonRays::Buffer* ray_buffer;
	RadeonRays::Buffer* isect_buffer;
};

bool init_data(Scene& scene)
{
	std::string basepath = "../Resources/CornellBox/";
	std::string filename = basepath + "orig.objm";
	std::string res = LoadObj(scene.g_objshapes, scene.g_objmaterials, filename.c_str(), basepath.c_str());
	return res != "" ? false : true;
}

bool init_RadeonRays(Scene& scene)
{
	RadeonRays::IntersectionApi::SetPlatform(RadeonRays::DeviceInfo::kAny);

	int deviceidx = -1;
	for (auto idx = 0U; idx < RadeonRays::IntersectionApi::GetDeviceCount(); ++idx)
	{
		RadeonRays::DeviceInfo devinfo;
		RadeonRays::IntersectionApi::GetDeviceInfo(idx, devinfo);

		if (devinfo.type == RadeonRays::DeviceInfo::kCpu)
			deviceidx = idx;
	}

	if (deviceidx == -1) return false;

	scene.api = RadeonRays::IntersectionApi::Create(deviceidx);
	return true;
}

bool init_RadeonRays_Scene(Scene& scene)
{
	for (int id = 0; id < scene.g_objshapes.size(); ++id)
	{
		tinyobj::shape_t& objshape = scene.g_objshapes[id];

		float* vertdata = objshape.mesh.positions.data();
		int nvert = objshape.mesh.positions.size();
		int* indices = objshape.mesh.indices.data();
		int nfaces = objshape.mesh.indices.size() / 3;

		RadeonRays::Shape* shape = scene.api->CreateMesh(vertdata, nvert, 3 * sizeof(float), indices, 0, nullptr, nfaces);

		assert(shape != nullptr);
		scene.api->AttachShape(shape);

		shape->SetId(id);
	}

	scene.api->Commit();

	return true;
}

bool init_RadeonRays_Camera(Scene& scene)
{
	const int k_raypack_size = scene.g_window_height * scene.g_window_width;

	// Prepare rays. One for each texture pixel.
	std::vector<RadeonRays::ray> rays(k_raypack_size);
	RadeonRays::float4 camera_pos = { 0.f, 1.f, 3.f, 1000.f };

	for (int i = 0; i < scene.g_window_height; ++i)
	{
		for (int j = 0; j < scene.g_window_width; ++j)
		{
			const float xstep = 2.f / (float)scene.g_window_width;
			const float ystep = 2.f / (float)scene.g_window_height;
			float x = -1.f + xstep * (float)j;
			float y = ystep * (float)i;
			float z = 1.f;

			rays[i * scene.g_window_width + j].o = camera_pos;
			rays[i * scene.g_window_width + j].d = RadeonRays::float3(x - camera_pos.x, y - camera_pos.y, z - camera_pos.z);
		}
	}

	scene.ray_buffer = scene.api->CreateBuffer(sizeof(RadeonRays::ray) * k_raypack_size, rays.data());
	scene.isect_buffer = scene.api->CreateBuffer(sizeof(RadeonRays::Intersection) * k_raypack_size, nullptr);

	return true;
}

RadeonRays::float3 ConvertFromBarycentric(const float* vec, const int* ind, int prim_id, const RadeonRays::float4& uvwt)
{
	RadeonRays::float3 a = { vec[ind[prim_id * 3] * 3], vec[ind[prim_id * 3] * 3 + 1], vec[ind[prim_id * 3] * 3 + 2], };
	RadeonRays::float3 b = { vec[ind[prim_id * 3 + 1] * 3], vec[ind[prim_id * 3 + 1] * 3 + 1], vec[ind[prim_id * 3 + 1] * 3 + 2], };
	RadeonRays::float3 c = { vec[ind[prim_id * 3 + 2] * 3], vec[ind[prim_id * 3 + 2] * 3 + 1], vec[ind[prim_id * 3 + 2] * 3 + 2], };
	return a * (1 - uvwt.x - uvwt.y) + b * uvwt.x + c * uvwt.y;
}

void dumpTGA(std::ostream& stream, std::uint8_t pixesl[], std::uint32_t width, std::uint32_t height, std::uint32_t channel) noexcept
{
	assert(stream);
	assert(width < std::numeric_limits<std::uint16_t>::max() || height < std::numeric_limits<std::uint16_t>::max());

	std::uint8_t  id_length = 0;
	std::uint8_t  colormap_type = 0;
	std::uint8_t  image_type = 2;
	std::uint16_t colormap_index = 0;
	std::uint16_t colormap_length = 0;
	std::uint8_t  colormap_size = 0;
	std::uint16_t x_origin = 0;
	std::uint16_t y_origin = 0;
	std::uint16_t w = (std::uint16_t)width;
	std::uint16_t h = (std::uint16_t)height;
	std::uint8_t  pixel_size = channel * 8;
	std::uint8_t  attributes = channel == 4 ? 8 : 0;

	stream.write((char*)&id_length, sizeof(id_length));
	stream.write((char*)&colormap_type, sizeof(colormap_type));
	stream.write((char*)&image_type, sizeof(image_type));
	stream.write((char*)&colormap_index, sizeof(colormap_index));
	stream.write((char*)&colormap_length, sizeof(colormap_length));
	stream.write((char*)&colormap_size, sizeof(colormap_size));
	stream.write((char*)&x_origin, sizeof(x_origin));
	stream.write((char*)&y_origin, sizeof(y_origin));
	stream.write((char*)&w, sizeof(w));
	stream.write((char*)&h, sizeof(h));
	stream.write((char*)&pixel_size, sizeof(pixel_size));
	stream.write((char*)&attributes, sizeof(attributes));

	std::vector<std::uint8_t> buffer(width * height * channel);
	for (std::size_t i = 0; i < buffer.size(); i++)
		buffer[i] = pixesl[i];

	stream.write((char*)buffer.data(), width * height * channel);
}

void dumpTGA(const char* filepath, std::uint8_t pixesl[], std::uint32_t width, std::uint32_t height, std::uint32_t channel) noexcept(false)
{
	auto stream = std::ofstream(filepath, std::ios_base::out | std::ios_base::binary);
	if (stream)
		dumpTGA(stream, pixesl, width, height, channel);
	else
		throw std::runtime_error("failed to open the file: " + std::string(filepath));
}

int main()
{
	Scene scene;
	if (!init_data(scene)) return false;
	if (!init_RadeonRays(scene)) return false;
	if (!init_RadeonRays_Scene(scene)) return false;
	if (!init_RadeonRays_Camera(scene)) return false;

	const int k_raypack_size = scene.g_window_height * scene.g_window_width;
	scene.api->QueryIntersection(scene.ray_buffer, k_raypack_size, scene.isect_buffer, nullptr, nullptr);

	// Get results
	RadeonRays::Event* e = nullptr;
	RadeonRays::Intersection* isect = nullptr;
	scene.api->MapBuffer(scene.isect_buffer, RadeonRays::kMapRead, 0, k_raypack_size * sizeof(RadeonRays::Intersection), (void**)&isect, &e);

	e->Wait();
	scene.api->DeleteEvent(e);
	e = nullptr;
	
	// Render triangle and lightning
	std::vector<std::uint32_t> tex_data(k_raypack_size);

	for (int i = 0; i < k_raypack_size; ++i)
	{
		int shape_id = isect[i].shapeid;
		int prim_id = isect[i].primid;

		if (shape_id != RadeonRays::kNullId && prim_id != RadeonRays::kNullId)
		{
			tinyobj::mesh_t& mesh = scene.g_objshapes[shape_id].mesh;
			tinyobj::material_t& mat = scene.g_objmaterials[mesh.material_ids[prim_id]];
		
			RadeonRays::float3 diff = { mat.diffuse[0], mat.diffuse[1], mat.diffuse[2] };
			RadeonRays::float3 pos = ConvertFromBarycentric(mesh.positions.data(), mesh.indices.data(), prim_id, isect[i].uvwt);
			RadeonRays::float3 norm = ConvertFromBarycentric(mesh.normals.data(), mesh.indices.data(), prim_id, isect[i].uvwt);
			norm.normalize();

			// Calculate lighting
			RadeonRays::float3 light_dir = scene.light - pos;
			light_dir.normalize();

			RadeonRays::float3 col = diff *std::max(0.0f, RadeonRays::dot(norm, light_dir));

			std::uint32_t color = 0xFF << 24;
			color |= (std::uint8_t)(255 * col[0]) << 16;
			color |= (std::uint8_t)(255 * col[1]) << 8;
			color |= (std::uint8_t)(255 * col[2]);

			tex_data[i] = color;
		}
	}

	dumpTGA("C:/Users/Administrator/Desktop/test.tga", (std::uint8_t*)tex_data.data(), scene.g_window_width, scene.g_window_height, 4);

    return 0;
}