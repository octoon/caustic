#ifndef OCTOON_CAUSTIC_MATERIAL_H_
#define OCTOON_CAUSTIC_MATERIAL_H_

#include <radeon_rays.h>

namespace octoon
{
	namespace caustic
	{
		class Material
		{
		public:
			RadeonRays::float3 albedo;
			RadeonRays::float3 specular;
			RadeonRays::float3 emissive;

			float ior;
			float roughness;
			float metalness;

			bool isEmissive() const noexcept
			{
				return emissive.x != 0.0f || emissive.y != 0.0f || emissive.z != 0.0f;
			}
		};
	}
}

#endif