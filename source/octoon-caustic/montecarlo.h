#ifndef OCTOON_CAUSTIC_MONTECARLO_H_
#define OCTOON_CAUSTIC_MONTECARLO_H_

#include <algorithm>
#include <vector>
#include <atomic>
#include <memory>
#include <radeon_rays.h>
#include <radeon_rays_cl.h>
#include <memory>
#include "tiny_obj_loader.h"

#include <octoon/caustic/BxDF.h>
#include <octoon/caustic/tonemapping.h>
#include <octoon/caustic/material.h>

namespace octoon
{
	struct RenderData
	{
		std::uint32_t numEstimate;

		std::vector<RadeonRays::ray> rays;
		std::vector<RadeonRays::Intersection> hits;
		std::vector<RadeonRays::Intersection> shadowHits;
		std::vector<RadeonRays::float3> samples;
		std::vector<RadeonRays::float2> random;
		std::vector<RadeonRays::float3> weights;

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

		const std::uint32_t* data() const noexcept;

		void render(std::uint32_t frame, const RadeonRays::int2& offset, const RadeonRays::int2& size) noexcept;

	private:
		bool init_data();
		bool init_Gbuffers(std::uint32_t w, std::uint32_t h) noexcept;
		bool init_RadeonRays() noexcept;
		bool init_RadeonRays_Scene();

	private:
		void GenerateWorkspace(std::uint32_t numEstimate);

		void GenerateNoise(std::uint32_t frame, const RadeonRays::int2& offset, const RadeonRays::int2& size) noexcept;
		void GenerateRays(std::uint32_t frame) noexcept;
		void GenerateFirstRays(std::uint32_t frame, const RadeonRays::int2& offset, const RadeonRays::int2& size) noexcept;

		void GatherFirstSampling(std::atomic_uint32_t& sampleCounter) noexcept;
		void GatherSampling(std::uint32_t pass) noexcept;
		void GatherHits() noexcept;
		void GatherShadowHits() noexcept;
		void GatherLightSamples() noexcept;

		void AccumSampling(std::uint32_t frame, const RadeonRays::int2& offset, const RadeonRays::int2& size) noexcept;
		void AdaptiveSampling() noexcept;

		void ColorTonemapping(std::uint32_t frame, const RadeonRays::int2& offset, const RadeonRays::int2& size) noexcept;

		void Estimate(std::uint32_t frame, const RadeonRays::int2& offset, const RadeonRays::int2& size);

	private:
		std::uint32_t width_;
		std::uint32_t height_;

		std::uint32_t numBounces_;
		std::uint32_t tileNums_;

		RadeonRays::IntersectionApi* api_;

		RadeonRays::float3 camera_;
		RadeonRays::float3 skyColor_;
		RadeonRays::float3 light_;

		std::vector<std::uint32_t> ldr_;
		std::vector<RadeonRays::float3> hdr_;

		RenderData renderData_;

		std::unique_ptr<caustic::BxDF> bxdf_;
		std::unique_ptr<caustic::Tonemapping> tonemapping_;
		std::unique_ptr<class CranleyPatterson> randomSampler_;

		std::vector<tinyobj::shape_t> scene_;
		std::vector<Material> materials_;
	};
}

#endif