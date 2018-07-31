#include <radeon_rays.h>
#include <assert.h>
#include <fstream>

using namespace RadeonRays;

namespace 
{
	float const g_vertices[] = {
		-1.f,-1.f,0.f,
		1.f,-1.f,0.f,
		0.f,1.f,0.f,
	};
	float const g_normals[] = {
		0.f,0.f,1.f,
		0.f,0.f,1.f,
		0.f,0.f,1.f,
	};
	int const g_indices[] = { 0, 1, 2 };
	const int g_numfaceverts[] = { 3 };
	const float3 g_color = { 1.f, 0.f, 0.f, 1.f };
	unsigned int g_vertex_buffer, g_index_buffer;
	unsigned int g_texture;
	int g_window_width = 640;
	int g_window_height = 480;
}

float3 ConvertFromBarycentric(const float* vec, const int* ind, int prim_id, const float4& uvwt)
{
	float3 a = { vec[ind[prim_id * 3] * 3],
		vec[ind[prim_id * 3] * 3 + 1],
		vec[ind[prim_id * 3] * 3 + 2], };

	float3 b = { vec[ind[prim_id * 3 + 1] * 3],
		vec[ind[prim_id * 3 + 1] * 3 + 1],
		vec[ind[prim_id * 3 + 1] * 3 + 2], };

	float3 c = { vec[ind[prim_id * 3 + 2] * 3],
		vec[ind[prim_id * 3 + 2] * 3 + 1],
		vec[ind[prim_id * 3 + 2] * 3 + 2], };
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
	auto stream = std::ofstream(filepath, std::ios_base::out);
	if (stream)
		dumpTGA(stream, pixesl, width, height, channel);
	else
		throw std::runtime_error("failed to open the file: " + std::string(filepath));
}

int main()
{
	IntersectionApi::SetPlatform(DeviceInfo::kAny);

	int deviceidx = -1;
	for (auto idx = 0U; idx < IntersectionApi::GetDeviceCount(); ++idx)
	{
		DeviceInfo devinfo;
		IntersectionApi::GetDeviceInfo(idx, devinfo);

		if (devinfo.type == DeviceInfo::kCpu)
			deviceidx = idx;
	}

	if (deviceidx == -1) return 1;

	IntersectionApi* api = IntersectionApi::Create(deviceidx);

	Shape* shape = api->CreateMesh(g_vertices, 3, 3 * sizeof(float), g_indices, 0, g_numfaceverts, 1);

	api->AttachShape(shape);
	api->Commit();

	const int k_raypack_size = g_window_height * g_window_width;

	// Prepare rays. One for each texture pixel.
	std::vector<ray> rays(k_raypack_size);

	for (int i = 0; i < g_window_height; ++i)
	{
		for (int j = 0; j < g_window_width; ++j)
		{
			const float xstep = 2.f / (float)g_window_width;
			const float ystep = 2.f / (float)g_window_height;
			float x = -1.f + xstep * (float)j;
			float y = -1.f + ystep * (float)i;
			float z = 1.f;
			rays[i * g_window_width + j].o = float3(x, y, z, 1000.f);
			rays[i * g_window_width + j].d = float3{ 0.f, 0.f, -1.f };
		}
	}

	Buffer* ray_buffer = api->CreateBuffer(sizeof(ray) * k_raypack_size, rays.data());
	Buffer* isect_buffer = api->CreateBuffer(sizeof(Intersection) * k_raypack_size, nullptr);

	api->QueryIntersection(ray_buffer, k_raypack_size, isect_buffer, nullptr, nullptr);

	// Get results
	Event* e = nullptr;
	Intersection* isect = nullptr;
	api->MapBuffer(isect_buffer, kMapRead, 0, k_raypack_size * sizeof(Intersection), (void**)&isect, &e);

	e->Wait();
	api->DeleteEvent(e);
	e = nullptr;

	// Point light position
	float3 light = { 0.f, 0.f, 0.25f };

	// Render triangle and lightning
	std::vector<std::uint8_t> tex_data(k_raypack_size * 4);
	for (int i = 0; i < k_raypack_size; ++i)
	{
		int shape_id = isect[i].shapeid;
		int prim_id = isect[i].primid;

		if (shape_id != kNullId)
		{
			// Calculate position and normal of the intersection point
			float3 pos = ConvertFromBarycentric(g_vertices, g_indices, prim_id, isect[i].uvwt);
			float3 norm = ConvertFromBarycentric(g_normals, g_indices, prim_id, isect[i].uvwt);
			norm.normalize();

			// Calculate lighting
			float3 col = { 0.f, 0.f, 0.f };
			float3 light_dir = light - pos;
			light_dir.normalize();
			float dot_prod = dot(norm, light_dir);
			if (dot_prod > 0)
				col += dot_prod * g_color;

			tex_data[i * 4] = col[0] * 255;
			tex_data[i * 4 + 1] = col[1] * 255;
			tex_data[i * 4 + 2] = col[2] * 255;
			tex_data[i * 4 + 3] = 255;
		}
		else
		{
			// Draw white pixels for misses
			tex_data[i * 4] = 255;
			tex_data[i * 4 + 1] = 255;
			tex_data[i * 4 + 2] = 255;
			tex_data[i * 4 + 3] = 255;
		}
	}

	dumpTGA("C:/Users/Administrator/Desktop/test.tga", tex_data.data(), g_window_width, g_window_height, 4);

    return 0;
}