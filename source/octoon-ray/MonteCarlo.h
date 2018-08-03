#ifndef OCTOON_MONTECARLO_H_
#define OCTOON_MONTECARLO_H_

#include <algorithm>
#include <vector>
#include <radeon_rays.h>
#include <stack>
#include <queue>
#include <thread>
#include <future>

#include "tiny_obj_loader.h"

namespace octoon
{
	class MonteCarlo
	{
	public:
		MonteCarlo() noexcept;
		MonteCarlo(std::uint32_t w, std::uint32_t h) noexcept;
		~MonteCarlo() noexcept;

		void setup(std::uint32_t w, std::uint32_t h) noexcept(false);

		const std::uint32_t* raw_data(std::uint32_t y) const noexcept;

		void query() noexcept;
		void render(std::uint32_t y, std::uint32_t frame) noexcept;

	private:
		bool init_data();
		bool init_Gbuffers(std::uint32_t w, std::uint32_t h) noexcept;
		bool init_RadeonRays() noexcept;
		bool init_RadeonRays_Camera();
		bool init_RadeonRays_Scene();

	private:
		RadeonRays::float3 PathTracing(RadeonRays::float3 ro, RadeonRays::float3 rd, RadeonRays::float3 norm, float roughness, float ior, std::uint32_t seed);
		RadeonRays::float3 MultPathTracing(const RadeonRays::float3& ro, const RadeonRays::float3& rd, const RadeonRays::float3& norm, float roughness, float ior, std::uint32_t bounce);

	private:
		std::uint32_t width_;
		std::uint32_t height_;

		std::uint32_t numBounces_;
		std::uint32_t numSamples_;

		RadeonRays::IntersectionApi* api_;
		RadeonRays::Buffer* ray_;
		RadeonRays::Buffer* hit_;
		RadeonRays::Buffer* ray_buffer_;
		RadeonRays::Buffer* isect_buffer_;

		RadeonRays::float3 camera_;
		RadeonRays::float3 skyColor_;
		RadeonRays::float3 light_;

		std::vector<std::uint32_t> ldr_;
		std::vector<RadeonRays::float3> hdr_;

		std::vector<std::uint8_t> hits_;
		std::vector<RadeonRays::ray> view_;
		std::vector<RadeonRays::float4> albede_;
		std::vector<RadeonRays::float4> normals_;
		std::vector<RadeonRays::float4> position_;

		std::vector<tinyobj::shape_t> g_objshapes;
		std::vector<tinyobj::material_t> g_objmaterials;
	};
}

#endif