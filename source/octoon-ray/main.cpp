#include <radeon_rays.h>

using namespace RadeonRays;



int main()
{
	int deviceidx = -1;
	for (auto idx = 0U; idx < IntersectionApi::GetDeviceCount(); ++idx)
	{
		DeviceInfo devinfo;
		IntersectionApi::GetDeviceInfo(idx, devinfo);

		if (devinfo.type == DeviceInfo::kGpu && devinfo.platform == DeviceInfo::kOpenCL)
		{
			deviceidx = idx;
		}
	}

	if (deviceidx == -1) return 1;

	IntersectionApi* api = IntersectionApi::Create(deviceidx);


    return 0;
}