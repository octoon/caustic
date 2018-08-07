#ifndef OCTOON_CAUSTIC_MATERIAL_H_
#define OCTOON_CAUSTIC_MATERIAL_H_

#include <radeon_rays.h>

namespace octoon
{
	struct Material
	{
		RadeonRays::float3 albedo;
		RadeonRays::float3 specular;
		RadeonRays::float3 emissive;

		float ior;
		float roughness;
		float metalness;
	};
}

#endif