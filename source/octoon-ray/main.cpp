#include <radeon_rays.h>
#include <assert.h>
#include <fstream>
#include <GLFW/glfw3.h>
#include <GL/GL.h>
#include "tiny_obj_loader.h"

struct Scene
{
	std::vector<tinyobj::shape_t> g_objshapes;
	std::vector<tinyobj::material_t> g_objmaterials;

	unsigned g_vertex_buffer, g_index_buffer;
	unsigned g_texture;

	GLFWwindow* window = nullptr;
	int width = 640;
	int height = 480;

	GLuint texture = 0;

	std::uint32_t spp = 10;
	std::uint32_t bounce = 2;

	// Point light position
	RadeonRays::float3 light = { -0.01f, 1.9f, 0.1f };
	RadeonRays::float3 sky = { 2.0f, 2.0f, 2.0f };
	RadeonRays::float3 camera = { 0.f, 1.f, 3.f, 1000.f };

	RadeonRays::IntersectionApi* api;
	RadeonRays::Buffer* ray_buffer;
	RadeonRays::Buffer* isect_buffer;

	std::vector<RadeonRays::ray> rays;

	std::vector<std::uint32_t> ldr;
	std::vector<RadeonRays::float3> hdr;
};

bool init_data(Scene& scene)
{
	std::string basepath = "../Resources/CornellBox/";
	std::string filename = basepath + "orig.objm";
	std::string res = LoadObj(scene.g_objshapes, scene.g_objmaterials, filename.c_str(), basepath.c_str());

	scene.ldr.resize(scene.width * scene.height);
	scene.hdr.resize(scene.width * scene.height);

	return res != "" ? false : true;
}

bool init_Window(Scene& scene)
{
	if (::glfwInit() == GL_FALSE)
		return false;

	scene.window = ::glfwCreateWindow(scene.width, scene.height, "Octoon Ray", nullptr, nullptr);
	::glfwMakeContextCurrent(scene.window);

	return scene.window != nullptr;
}

bool init_RadeonRays(Scene& scene)
{
	RadeonRays::IntersectionApi::SetPlatform(RadeonRays::DeviceInfo::kAny);

	int deviceidx = -1;
	for (auto idx = 0U; idx < RadeonRays::IntersectionApi::GetDeviceCount(); ++idx)
	{
		RadeonRays::DeviceInfo devinfo;
		RadeonRays::IntersectionApi::GetDeviceInfo(idx, devinfo);

		if (devinfo.type == RadeonRays::DeviceInfo::kGpu)
		{
			deviceidx = idx;
			break;
		}

		if (devinfo.type == RadeonRays::DeviceInfo::kCpu)
		{
			deviceidx = idx;
			break;
		}
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
	const int k_raypack_size = scene.height * scene.width;

	// Prepare rays. One for each texture pixel.
	std::vector<RadeonRays::ray> rays(k_raypack_size);

#pragma omp parallel for
	for (int i = 0; i < scene.height; ++i)
	{
		for (int j = 0; j < scene.width; ++j)
		{
			const float xstep = 2.f / (float)scene.width;
			const float ystep = 2.f / (float)scene.height;
			float x = -1.f + xstep * (float)j;
			float y = ystep * (float)i;
			float z = 1.f;

			rays[i * scene.width + j].o = scene.camera;
			rays[i * scene.width + j].d = RadeonRays::float3(x - scene.camera.x, y - scene.camera.y, z - scene.camera.z);
			rays[i * scene.width + j].d.normalize();
		}
	}

	scene.rays = rays;
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

inline RadeonRays::float3 reflect(const RadeonRays::float3& I, const RadeonRays::float3& N) noexcept
{
	return I - 2 * (RadeonRays::dot(I, N) * N);
}

inline float TonemapACES(float x)
{
	const float A = 2.51f;
	const float B = 0.03f;
	const float C = 2.43f;
	const float D = 0.59f;
	const float E = 0.14f;
	return std::min(1.0f, (x * (A * x + B)) / (x * (C * x + D) + E));
}

float GetPhysicalLightAttenuation(const RadeonRays::float3& L, float radius = std::numeric_limits<float>::max(), float attenuationBulbSize = 1.0f)
{
	const float invRadius = 1 / radius;
	float d = std::sqrt(RadeonRays::dot(L, L));
	float fadeoutFactor = std::min(1.0f, std::max(0.0f, (radius - d) * (invRadius / 0.2f)));
	d = std::max(d - attenuationBulbSize, 0.0f);
	float denom = 1 + d / attenuationBulbSize;
	float attenuation = fadeoutFactor * fadeoutFactor / (denom * denom);
	return attenuation;
}

std::uint32_t ReverseBits32(std::uint32_t bits)
{
	bits = (bits << 16) | (bits >> 16);
	bits = ((bits & 0x00ff00ff) << 8) | ((bits & 0xff00ff00) >> 8);
	bits = ((bits & 0x0f0f0f0f) << 4) | ((bits & 0xf0f0f0f0) >> 4);
	bits = ((bits & 0x33333333) << 2) | ((bits & 0xcccccccc) >> 2);
	bits = ((bits & 0x55555555) << 1) | ((bits & 0xaaaaaaaa) >> 1);
	return bits;
}

RadeonRays::float2 Hammersley(std::uint32_t i, std::uint32_t samplesCount)
{
	float E1 = (float)i / samplesCount;
	float E2 = ReverseBits32(i) * 2.3283064365386963e-10f;
	return RadeonRays::float2(E1, E2);
}

RadeonRays::float2 Hammersley(std::uint32_t i, std::uint32_t samplesCount, std::uint32_t seed)
{
	float E1 = (float)i / samplesCount + float(seed & 0xffff) / (1 << 16);
	float E2 = (ReverseBits32(i) ^ seed) * 2.3283064365386963e-10f;
	return RadeonRays::float2(E1 - std::floor(E1), E2);
}

RadeonRays::float3 SampleHemisphereUniform(const RadeonRays::float2& Xi)
{
	float phi = Xi.y * 2.0 * PI;
	float cosTheta = 1.0 - Xi.x;
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

	return RadeonRays::float3(std::cos(phi) * sinTheta, cosTheta, std::sin(phi) * sinTheta);
}

RadeonRays::float3 HammersleySampleCos(const RadeonRays::float2& Xi)
{
	float phi = Xi.x * 2.0f * 3.141592654f;

	float cosTheta = std::sqrt(Xi.y);
	float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);

	RadeonRays::float3 H;
	H.x = std::cos(phi) * sinTheta;
	H.y = std::sin(phi) * sinTheta;
	H.z = cosTheta;

	return H;
}

RadeonRays::float3 HammersleySampleGGX(const RadeonRays::float2& Xi, float roughness)
{
	float m = roughness * roughness;
	float m2 = m * m;
	float u = (1.0f - Xi.y) / (1.0f + (m2 - 1) * Xi.y);

	return HammersleySampleCos(RadeonRays::float2(Xi.x, u));
}

RadeonRays::float3 HammersleySampleLambert(const RadeonRays::float3& n, std::uint32_t i, std::uint32_t samplesCount, std::uint32_t seed)
{
	auto H = HammersleySampleCos(Hammersley(i, samplesCount, seed));
	H.z = H.z * 2.0f - 1.0f;
	H += n;
	H.normalize();

	return H;
}

RadeonRays::float3 TangentToWorld(const RadeonRays::float3& N, const RadeonRays::float3& H)
{
	RadeonRays::float3 Y = std::abs(N.z) < 0.999f ? RadeonRays::float3(0,0,1) : RadeonRays::float3(1,0,0);
	RadeonRays::float3 X = RadeonRays::cross(Y, N);
	X.normalize();
	return X * H.x + cross(N, X) * H.y + N * H.z;
}

float hash(float seed)
{
	float noise = std::sin(seed) * 43758.5453;
	return noise - std::floor(noise);
};

RadeonRays::float3 CosineDirection(const RadeonRays::float3& n, float seed)
{
	float u = hash(78.233 + seed);
	float v = hash(10.873 + seed);

	RadeonRays::float3 H = HammersleySampleCos(RadeonRays::float2(v, u));
	H.z = H.z * 2.0f - 1.0f;
	H += n;
	H.normalize();

	return H;
}

RadeonRays::float3 bsdf(const RadeonRays::float3& n, std::uint32_t i, std::uint32_t samplesCount, std::uint64_t seed)
{
	return CosineDirection(n, hash(seed) + float(i) / samplesCount);
}

RadeonRays::float3 PathTracing(Scene& scene, const RadeonRays::float3& ro, const RadeonRays::float3& norm, std::uint64_t seed, std::uint32_t bounce)
{
	if (bounce > scene.bounce)
		return RadeonRays::float3(0.0f, 0.0f, 0.0f);

	RadeonRays::ray* rays = nullptr;
	RadeonRays::Event* e = nullptr;
	RadeonRays::Intersection* hits = nullptr;
	RadeonRays::float3 colorAccum(0.0f, 0.0f, 0.0f);

	auto spp_ray = scene.api->CreateBuffer(sizeof(RadeonRays::ray) * scene.spp, nullptr);
	auto spp_hit = scene.api->CreateBuffer(sizeof(RadeonRays::Intersection) * scene.spp, nullptr);

	scene.api->MapBuffer(spp_ray, RadeonRays::kMapWrite, 0, sizeof(RadeonRays::ray) * scene.spp, (void**)&rays, &e); e->Wait(); scene.api->DeleteEvent(e);

#pragma omp parallel for
	for (auto i = 0; i < scene.spp; i++)
	{
		RadeonRays::float3 rd = bsdf(norm, i, scene.spp, seed);
		rd.normalize();

		auto& ray = rays[i];
		ray.o = ro + rd * 1e-4f;
		ray.d = rd;
		ray.SetMaxT(std::numeric_limits<float>::max());
		ray.SetTime(0.0f);
		ray.SetMask(-1);
		ray.SetActive(true);
		ray.SetDoBackfaceCulling(true);
	}

	scene.api->UnmapBuffer(spp_ray, rays, &e); e->Wait(); scene.api->DeleteEvent(e);
	scene.api->QueryIntersection(spp_ray, scene.spp, spp_hit, nullptr, &e); e->Wait(); scene.api->DeleteEvent(e);
	scene.api->MapBuffer(spp_hit, RadeonRays::kMapRead, 0, sizeof(RadeonRays::Intersection) * scene.spp, (void**)&hits, &e); e->Wait(); scene.api->DeleteEvent(e);

	for (std::size_t i = 0; i < scene.spp; i++)
	{
		auto& hit = hits[i];
		if (hit.shapeid != RadeonRays::kNullId && hit.primid != RadeonRays::kNullId)
		{
			tinyobj::mesh_t& mesh = scene.g_objshapes[hit.shapeid].mesh;
			tinyobj::material_t& mat = scene.g_objmaterials[mesh.material_ids[hit.primid]];

			if (mat.emission[0] > 0.0f || mat.emission[1] > 0.0f || mat.emission[2] > 0.0f)
			{
				colorAccum += RadeonRays::float3(mat.emission[0], mat.emission[1], mat.emission[2]);
			}
			else
			{
				RadeonRays::float3 diff(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]);
				auto p = ConvertFromBarycentric(mesh.positions.data(), mesh.indices.data(), hit.primid, hit.uvwt);
				auto n = ConvertFromBarycentric(mesh.normals.data(), mesh.indices.data(), hit.primid, hit.uvwt);
				auto atten = GetPhysicalLightAttenuation(p - ro);

				colorAccum += diff * atten * PathTracing(scene, p, n, seed, ++bounce);
			}
		}
		else
		{
			colorAccum += scene.sky;
		}
	}

	scene.api->UnmapBuffer(spp_hit, hits, nullptr);

	scene.api->DeleteBuffer(spp_ray);
	scene.api->DeleteBuffer(spp_hit);

	return colorAccum;
}

int main()
{
	Scene scene;
	if (!init_data(scene)) return false;
	if (!init_Window(scene)) return false;
	if (!init_RadeonRays(scene)) return false;
	if (!init_RadeonRays_Scene(scene)) return false;
	if (!init_RadeonRays_Camera(scene)) return false;

	RadeonRays::Event* e = nullptr;
	RadeonRays::Intersection* isect = nullptr;

	scene.api->QueryIntersection(scene.ray_buffer, scene.height * scene.width, scene.isect_buffer, nullptr, nullptr);
	scene.api->MapBuffer(scene.isect_buffer, RadeonRays::kMapRead, 0, scene.height * scene.width * sizeof(RadeonRays::Intersection), (void**)&isect, &e); e->Wait(); scene.api->DeleteEvent(e);

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &scene.texture);
	glBindTexture(GL_TEXTURE_2D, scene.texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, scene.width, scene.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, GL_NONE);

	for (std::uint32_t frame = 1; ; frame++)
	{
		for (std::int32_t y = scene.height - 1; y > 0; y--)
		{
			for (std::uint32_t x = 0; x < scene.width; ++x)
			{
				int i = y * scene.width + x;
				int shape_id = isect[i].shapeid;
				int prim_id = isect[i].primid;

				if (shape_id == RadeonRays::kNullId || prim_id == RadeonRays::kNullId)
					continue;

				tinyobj::mesh_t& mesh = scene.g_objshapes[shape_id].mesh;
				tinyobj::material_t& mat = scene.g_objmaterials[mesh.material_ids[prim_id]];

				RadeonRays::float3 norm = ConvertFromBarycentric(mesh.normals.data(), mesh.indices.data(), prim_id, isect[i].uvwt);
				RadeonRays::float3 ro = ConvertFromBarycentric(mesh.positions.data(), mesh.indices.data(), prim_id, isect[i].uvwt);
				RadeonRays::float3 diffuse(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]);

				std::uint32_t bounce = 0;

				if (mat.emission[0] > 0.0f || mat.emission[1] > 0.0f || mat.emission[2] > 0.0f)
					scene.hdr[i] += RadeonRays::float3(mat.emission[0], mat.emission[1], mat.emission[2]);
				else
					scene.hdr[i] += diffuse * PathTracing(scene, ro, norm, i * frame, bounce) * (1.0f / scene.spp);
			}

			for (std::uint32_t x = 0; x < scene.width; ++x)
			{
				int i = y * scene.width + x;
				std::uint8_t r = TonemapACES(scene.hdr[i].x / frame) * 255;
				std::uint8_t g = TonemapACES(scene.hdr[i].y / frame) * 255;
				std::uint8_t b = TonemapACES(scene.hdr[i].z / frame) * 255;

				scene.ldr[i] = 0xFF << 24 | b << 16 | g << 8 | r;
			}

			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, y, scene.width, 1, GL_RGBA, GL_UNSIGNED_BYTE, &scene.ldr[y * scene.width]);

			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 0.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 0.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
			glEnd();

			glfwPollEvents();
			glfwSwapBuffers(scene.window);

			if (::glfwWindowShouldClose(scene.window))
			{
				RadeonRays::IntersectionApi::Delete(scene.api);
				std::exit(0);
			}
		}
	}

	
    return 0;
}