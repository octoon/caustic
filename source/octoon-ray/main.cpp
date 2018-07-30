#include <radeon_rays.h>

using namespace RadeonRays;



int main()
{
	int deviceidx = -1;
	for (auto idx = 0U; idx < IntersectionApi::GetDeviceCount(); ++idx)
	{
		DeviceInfo devinfo;
		IntersectionApi::GetDeviceInfo(idx, devinfo);

		if (devinfo.type == DeviceInfo::kGpu /*&& devinfo.platform == DeviceInfo::kOpenCL*/)
		{
			deviceidx = idx;
		}
	}

	if (deviceidx == -1) return 1;

	IntersectionApi* api = IntersectionApi::Create(deviceidx);

	// Mesh vertices
	float const g_vertices[] = {
		-1.f,-1.f,0.f,
		1.f,-1.f,0.f,
		0.f,1.f,0.f,
	};
	int const g_indices[] = { 0, 1, 2 };
	// Number of vertices for the face
	const int g_numfaceverts[] = { 3 };

	Shape* shape = api->CreateMesh(g_vertices, 3, 3 * sizeof(float), g_indices, 0, g_numfaceverts, 1);

	api->AttachShape(shape);
	//api->DetachShape(shape);

	ray rays[3];
	rays[0].o = float4(0.f, 0.f, -1.f, 1000.f);
	rays[0].d = float3(0.f, 0.f, 10.f);
	rays[1].o = float4(0.f, 0.5f, -10.f, 1000.f);
	rays[1].d = float3(0.f, 0.f, 1.f);
	rays[2].o = float4(0.4f, 0.f, -10.f, 1000.f);
	rays[2].d = float3(0.f, 0.f, 1.f);
	Buffer* ray_buffer = api->CreateBuffer(3 * sizeof(ray), rays);

	Buffer* isect_buffer = api->CreateBuffer(3 * sizeof(Intersection), nullptr);

    return 0;
}