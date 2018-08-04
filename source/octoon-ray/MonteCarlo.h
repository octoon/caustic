#ifndef OCTOON_MONTECARLO_H_
#define OCTOON_MONTECARLO_H_

#include <algorithm>
#include <vector>
#include <radeon_rays.h>
#include <radeon_rays_cl.h>
#include <stack>
#include <queue>
#include <thread>
#include <future>

#include "tiny_obj_loader.h"

namespace octoon
{
	struct RenderData
	{
		std::vector<RadeonRays::ray> rays;
		std::vector<RadeonRays::Intersection> hits;
		std::vector<RadeonRays::float4> albede_;
		std::vector<RadeonRays::float4> normals_;
		std::vector<RadeonRays::float4> position_;

		RadeonRays::Buffer* fr_rays;
		RadeonRays::Buffer* fr_shadowrays;
		RadeonRays::Buffer* fr_shadowhits;
		RadeonRays::Buffer* fr_hits;
		RadeonRays::Buffer* fr_intersections;
		RadeonRays::Buffer* fr_hitcount;
	};

	class MonteCarlo
	{
	public:
		MonteCarlo() noexcept;
		MonteCarlo(std::uint32_t w, std::uint32_t h) noexcept;
		~MonteCarlo() noexcept;

		void setup(std::uint32_t w, std::uint32_t h) noexcept(false);

		const std::uint32_t* raw_data(std::uint32_t y) const noexcept;

		void render(std::uint32_t frame, std::uint32_t y) noexcept;

	private:
		bool init_data();
		bool init_Gbuffers(std::uint32_t w, std::uint32_t h) noexcept;
		bool init_RadeonRays() noexcept;
		bool init_RadeonRays_Camera();
		bool init_RadeonRays_Scene();

	private:
		void GenerateRays(std::uint32_t frame, std::uint32_t y);
		void GenerateIntersection(std::uint32_t frame, std::uint32_t y) noexcept;

	private:
		void Estimate(std::uint32_t frame, std::uint32_t y);

	private:
		std::uint32_t width_;
		std::uint32_t height_;

		std::uint32_t numBounces_;
		std::uint32_t numSamples_;

		RadeonRays::IntersectionApi* api_;
		RadeonRays::Buffer* ray_buffer_;
		RadeonRays::Buffer* isect_buffer_;

		RadeonRays::float3 camera_;
		RadeonRays::float3 skyColor_;
		RadeonRays::float3 light_;

		std::vector<std::uint32_t> ldr_;
		std::vector<RadeonRays::float3> hdr_;
		std::vector<RadeonRays::float3> accum_;

		RenderData renderData_;

		std::vector<RadeonRays::ray> view_;

		std::vector<tinyobj::shape_t> scene_;
		std::vector<tinyobj::material_t> materials_;
	};
}

#endif