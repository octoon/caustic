#include "MonteCarlo.h"
#include "Hammersley.h"
#include "BxDF.h"
#include <assert.h>
#include <atomic>
#include <string>
#include <CL/cl.h>

namespace octoon
{
	float TonemapACES(float x)
	{
		const float A = 2.51f;
		const float B = 0.03f;
		const float C = 2.43f;
		const float D = 0.59f;
		const float E = 0.14f;
		return std::pow(std::min(1.0f, (x * (A * x + B)) / (x * (C * x + D) + E)), 1.0f / 2.2f);
	}

	RadeonRays::float3 ConvertFromBarycentric(const float* vec, const int* ind, int prim_id, const RadeonRays::float4& uvwt)
	{
		RadeonRays::float3 a = { vec[ind[prim_id * 3] * 3], vec[ind[prim_id * 3] * 3 + 1], vec[ind[prim_id * 3] * 3 + 2], };
		RadeonRays::float3 b = { vec[ind[prim_id * 3 + 1] * 3], vec[ind[prim_id * 3 + 1] * 3 + 1], vec[ind[prim_id * 3 + 1] * 3 + 2], };
		RadeonRays::float3 c = { vec[ind[prim_id * 3 + 2] * 3], vec[ind[prim_id * 3 + 2] * 3 + 1], vec[ind[prim_id * 3 + 2] * 3 + 2], };
		return a * (1 - uvwt.x - uvwt.y) + b * uvwt.x + c * uvwt.y;
	}

	MonteCarlo::MonteCarlo() noexcept
		: camera_(0.f, 1.f, 3.f, 1000.f)
		, numBounces_(6)
		, tileNums_(0)
		, skyColor_(2.0f, 2.0f, 2.0f)
		, light_(-0.01f, 1.9f, 0.1f)
		, width_(0)
		, height_(0)
		, api_(nullptr)
	{
		renderData_.numEstimate = 0;
		renderData_.fr_rays = nullptr;
		renderData_.fr_hits = nullptr;
		renderData_.fr_intersections = nullptr;
		renderData_.fr_shadowrays = nullptr;
		renderData_.fr_shadowhits = nullptr;
	}

	MonteCarlo::MonteCarlo(std::uint32_t w, std::uint32_t h) noexcept
		: MonteCarlo()
	{
		this->setup(w, h);
	}

	MonteCarlo::~MonteCarlo() noexcept
	{
		if (renderData_.fr_rays)
			api_->DeleteBuffer(renderData_.fr_rays);
		if (renderData_.fr_hits)
			api_->DeleteBuffer(renderData_.fr_hits);
		if (renderData_.fr_intersections)
			api_->DeleteBuffer(renderData_.fr_intersections);
		if (renderData_.fr_shadowhits)
			api_->DeleteBuffer(renderData_.fr_shadowhits);
		if (renderData_.fr_shadowrays)
			api_->DeleteBuffer(renderData_.fr_shadowrays);
		if (api_)
			RadeonRays::IntersectionApi::Delete(api_);
	}

	void
	MonteCarlo::setup(std::uint32_t w, std::uint32_t h) noexcept(false)
	{
		width_ = w;
		height_ = h;

		haltonSampler_ = std::make_unique<Halton_sampler>();
		haltonSampler_->init_faure();

		if (!init_data()) throw std::runtime_error("init_data() fail");
		if (!init_Gbuffers(w, h)) throw std::runtime_error("init_Gbuffers() fail");
		if (!init_RadeonRays()) throw std::runtime_error("init_RadeonRays() fail");
		if (!init_RadeonRays_Scene()) throw std::runtime_error("init_RadeonRays_Scene() fail");
	}

	bool
	MonteCarlo::init_Gbuffers(std::uint32_t, std::uint32_t h) noexcept
	{
		auto allocSize = width_ * height_;
		auto rand = [](std::uint32_t seed) { return fract(std::sin(seed) * 43758.5453123); };

		ldr_.resize(allocSize);
		hdr_.resize(allocSize);
		random_.resize(allocSize);

#pragma omp parallel for
		for (std::int32_t i = 0; i < allocSize; ++i)
		{
			float sx = rand(i - 64.340622f);
			float sy = rand(i - 72.465622f);
			random_[i] = RadeonRays::float2(sx, sy);
		}

		return true;
	}

	bool
	MonteCarlo::init_RadeonRays() noexcept
	{
		RadeonRays::IntersectionApi::SetPlatform(RadeonRays::DeviceInfo::kAny);

		int deviceidx = -1;
		for (auto idx = 0U; idx < RadeonRays::IntersectionApi::GetDeviceCount(); ++idx)
		{
			RadeonRays::DeviceInfo devinfo;
			RadeonRays::IntersectionApi::GetDeviceInfo(idx, devinfo);

			if (devinfo.type == RadeonRays::DeviceInfo::kGpu)
			{
				std::string info_name(devinfo.name);
				if (info_name.find("Intel") != std::string::npos)
					continue;
				deviceidx = idx;
				break;
			}
		}

		if (deviceidx == -1)
		{
			for (auto idx = 0U; idx < RadeonRays::IntersectionApi::GetDeviceCount(); ++idx)
			{
				RadeonRays::DeviceInfo devinfo;
				RadeonRays::IntersectionApi::GetDeviceInfo(idx, devinfo);

				if (devinfo.type == RadeonRays::DeviceInfo::kCpu)
				{
					deviceidx = idx;
					break;
				}
			}
		}

		if (deviceidx == -1) return false;

		this->api_ = RadeonRays::IntersectionApi::Create(deviceidx);

		return true;
	}

	bool
	MonteCarlo::init_data()
	{
		std::string basepath = "../Resources/CornellBox/";
		std::string filename = basepath + "orig.objm";
		std::string res = LoadObj(scene_, materials_, filename.c_str(), basepath.c_str());

		for (auto& it : materials_)
		{
			it.diffuse[0] = std::pow(it.diffuse[0], 2.2f) * it.illum;
			it.diffuse[1] = std::pow(it.diffuse[1], 2.2f) * it.illum;
			it.diffuse[2] = std::pow(it.diffuse[2], 2.2f) * it.illum;

			it.specular[0] = std::pow(it.specular[0], 2.2f) * it.illum;
			it.specular[1] = std::pow(it.specular[1], 2.2f) * it.illum;
			it.specular[2] = std::pow(it.specular[2], 2.2f) * it.illum;

			it.emission[0] /= (4 * PI / it.illum);
			it.emission[1] /= (4 * PI / it.illum);
			it.emission[2] /= (4 * PI / it.illum);

			it.shininess = saturate(it.shininess);
		}

		return res != "" ? false : true;
	}

	bool
	MonteCarlo::init_RadeonRays_Scene()
	{
		for (int id = 0; id < this->scene_.size(); ++id)
		{
			tinyobj::shape_t& objshape = this->scene_[id];

			float* vertdata = objshape.mesh.positions.data();
			int nvert = objshape.mesh.positions.size() / 3;
			int* indices = objshape.mesh.indices.data();
			int nfaces = objshape.mesh.indices.size() / 3;

			RadeonRays::Shape* shape = this->api_->CreateMesh(vertdata, nvert, 3 * sizeof(float), indices, 0, nullptr, nfaces);

			assert(shape != nullptr);
			this->api_->AttachShape(shape);

			shape->SetId(id);
		}

		this->api_->Commit();

		return true;
	}

	const std::uint32_t*
	MonteCarlo::data() const noexcept
	{
		return ldr_.data();
	}

	void
	MonteCarlo::GenerateWorkspace(std::uint32_t numEstimate)
	{
		if (tileNums_ < numEstimate)
		{
			renderData_.rays.resize(numEstimate);
			renderData_.hits.resize(numEstimate);
			renderData_.samples.resize(numEstimate);
			renderData_.random.resize(numEstimate);
			renderData_.weights.resize(numEstimate);

			if (renderData_.fr_rays)
				api_->DeleteBuffer(renderData_.fr_rays);

			if (renderData_.fr_hits)
				api_->DeleteBuffer(renderData_.fr_hits);

			if (renderData_.fr_intersections)
				api_->DeleteBuffer(renderData_.fr_intersections);

			if (renderData_.fr_shadowrays)
				api_->DeleteBuffer(renderData_.fr_shadowrays);

			if (renderData_.fr_shadowhits)
				api_->DeleteBuffer(renderData_.fr_shadowhits);

			renderData_.fr_rays = api_->CreateBuffer(sizeof(RadeonRays::ray) * numEstimate, nullptr);
			renderData_.fr_hits = api_->CreateBuffer(sizeof(RadeonRays::Intersection) * numEstimate, nullptr);
			renderData_.fr_intersections = api_->CreateBuffer(sizeof(RadeonRays::Intersection) * numEstimate, nullptr);
			renderData_.fr_shadowrays = api_->CreateBuffer(sizeof(RadeonRays::ray) * numEstimate, nullptr);
			renderData_.fr_shadowhits = api_->CreateBuffer(sizeof(RadeonRays::Intersection) * numEstimate, nullptr);

			tileNums_ = numEstimate;
		}

		this->renderData_.numEstimate = numEstimate;
	}

	void
	MonteCarlo::GenerateNoise(std::uint32_t frame, const RadeonRays::int2& offset, const RadeonRays::int2& size) noexcept
	{
#pragma omp parallel for
		for (std::int32_t i = 0; i < this->renderData_.numEstimate; ++i)
		{
			auto ix = offset.x + i % size.x;
			auto iy = offset.y + i / size.x;
			auto index = iy * this->width_ + ix;

			float sx = fract(haltonSampler_->sample(0, frame) + random_[index].x);
			float sy = fract(haltonSampler_->sample(1, frame) + random_[index].y);

			this->renderData_.random[i] = RadeonRays::float2(sx, sy);
		}
	}

	void
	MonteCarlo::GenerateFirstRays(std::uint32_t frame, const RadeonRays::int2& offset, const RadeonRays::int2& size) noexcept
	{
		RadeonRays::ray* rays = nullptr;
		RadeonRays::Event* e = nullptr;
		api_->MapBuffer(renderData_.fr_rays, RadeonRays::kMapWrite, 0, sizeof(RadeonRays::ray) * this->renderData_.numEstimate, (void**)&rays, &e); e->Wait(); api_->DeleteEvent(e);

		float xstep = 2.0f / (float)this->width_;
		float ystep = 2.0f / (float)this->height_;

#pragma omp parallel for
		for (std::int32_t i = 0; i < this->renderData_.numEstimate; ++i)
		{
			auto ix = offset.x + i % size.x;
			auto iy = offset.y + i / size.x;

			float x = xstep * ix + (renderData_.random[i].x * 2 - 1) / (float)this->width_ - 1.0f;
			float y = ystep * iy + (renderData_.random[i].y / (float)this->height_);
			float z = 1.0f;

			auto& ray = rays[i];
			ray.o = this->camera_;
			ray.d = RadeonRays::float3((x - this->camera_.x) * width_ / height_, y - this->camera_.y, z - this->camera_.z);
			ray.d.normalize();
			ray.SetMaxT(std::numeric_limits<float>::max());
			ray.SetTime(0.0f);
			ray.SetMask(-1);
			ray.SetActive(true);
			ray.SetDoBackfaceCulling(true);
		}

		std::memcpy(renderData_.rays.data(), rays, sizeof(RadeonRays::ray) * this->renderData_.numEstimate);

		api_->UnmapBuffer(renderData_.fr_rays, rays, &e); e->Wait(); api_->DeleteEvent(e);
	}

	void
	MonteCarlo::GenerateRays(std::uint32_t frame) noexcept
	{
		RadeonRays::ray* rays = nullptr;
		RadeonRays::Event* e = nullptr;
		api_->MapBuffer(renderData_.fr_rays, RadeonRays::kMapWrite, 0, sizeof(RadeonRays::ray) * this->renderData_.numEstimate, (void**)&rays, &e); e->Wait(); api_->DeleteEvent(e);

		std::memset(renderData_.weights.data(), 0, sizeof(RadeonRays::float3) * this->renderData_.numEstimate);

#pragma omp parallel for
		for (std::int32_t i = 0; i < this->renderData_.numEstimate; ++i)
		{
			auto& hit = renderData_.hits[i];
			if (hit.shapeid != RadeonRays::kNullId && hit.primid != RadeonRays::kNullId)
			{
				tinyobj::mesh_t& mesh = scene_[hit.shapeid].mesh;
				tinyobj::material_t& mat = materials_[mesh.material_ids[hit.primid]];

				if (mat.emission[0] > 0.0f || mat.emission[1] > 0.0f || mat.emission[2] > 0.0f)
				{
					std::memset(&renderData_.rays[i], 0, sizeof(RadeonRays::ray));
				}
				else
				{
					auto ior = mat.ior;
					auto roughness = mat.shininess;
					auto ro = ConvertFromBarycentric(mesh.positions.data(), mesh.indices.data(), hit.primid, hit.uvwt);
					auto norm = ConvertFromBarycentric(mesh.normals.data(), mesh.indices.data(), hit.primid, hit.uvwt);

					RadeonRays::float3 L = bsdf(renderData_.rays[i].d, norm, roughness, ior, renderData_.random[i]);
					if (ior <= 1.0f)
					{
						if (RadeonRays::dot(norm, L) < 0.0f)
							L = -L;
					}

					assert(std::isfinite(L.x + L.y + L.z));

					renderData_.weights[i] = bsdf_weight(renderData_.rays[i].d, norm, L, RadeonRays::float3(mat.specular[0], mat.specular[1], mat.specular[2]), roughness, ior);

					auto& ray = renderData_.rays[i];
					ray.d = L;
					ray.o = ro + L * 1e-5f;
					ray.SetMaxT(std::numeric_limits<float>::max());
					ray.SetTime(0.0f);
					ray.SetMask(-1);
					ray.SetActive(true);
					ray.SetDoBackfaceCulling(ior > 1.0f ? false : true);
				}
			}
			else
			{
				std::memset(&renderData_.rays[i], 0, sizeof(RadeonRays::ray));
			}
		}

		std::memcpy(rays, renderData_.rays.data(), sizeof(RadeonRays::ray) * this->renderData_.numEstimate);

		api_->UnmapBuffer(renderData_.fr_rays, rays, &e); e->Wait(); api_->DeleteEvent(e);
	}

	void
	MonteCarlo::GatherHits() noexcept
	{
		RadeonRays::Event* e = nullptr;
		RadeonRays::Intersection* hit = nullptr;

		api_->MapBuffer(renderData_.fr_hits, RadeonRays::kMapRead, 0, sizeof(RadeonRays::Intersection) * this->renderData_.numEstimate, (void**)&hit, &e); e->Wait(); api_->DeleteEvent(e);

		std::memcpy(renderData_.hits.data(), hit, sizeof(RadeonRays::Intersection) * this->renderData_.numEstimate);

		api_->UnmapBuffer(renderData_.fr_hits, hit, &e); e->Wait(); api_->DeleteEvent(e);
	}

	void
	MonteCarlo::GatherShadowHits() noexcept
	{
		RadeonRays::Event* e = nullptr;
		RadeonRays::Intersection* hit = nullptr;

		api_->MapBuffer(renderData_.fr_shadowhits, RadeonRays::kMapRead, 0, sizeof(RadeonRays::Intersection) * this->renderData_.numEstimate, (void**)&hit, &e); e->Wait(); api_->DeleteEvent(e);

		std::memcpy(renderData_.hits.data(), hit, sizeof(RadeonRays::Intersection) * this->renderData_.numEstimate);

		api_->UnmapBuffer(renderData_.fr_shadowhits, hit, &e); e->Wait(); api_->DeleteEvent(e);
	}

	void
	MonteCarlo::GatherFirstSampling(std::atomic_uint32_t& sampleCounter) noexcept
	{
		std::memset(renderData_.samples.data(), 0, sizeof(RadeonRays::float3) * this->renderData_.numEstimate);

#pragma omp parallel for
		for (std::int32_t i = 0; i < this->renderData_.numEstimate; ++i)
		{
			if (!renderData_.rays[i].IsActive())
				continue;

			auto& hit = renderData_.hits[i];
			auto& sample = renderData_.samples[i];
			if (hit.shapeid == RadeonRays::kNullId || hit.primid == RadeonRays::kNullId)
				continue;

			tinyobj::mesh_t& mesh = scene_[hit.shapeid].mesh;
			tinyobj::material_t& mat = materials_[mesh.material_ids[hit.primid]];

			if (mat.emission[0] > 0.0f || mat.emission[1] > 0.0f || mat.emission[2] > 0.0f)
			{
				sample.x = mat.emission[0];
				sample.y = mat.emission[1];
				sample.z = mat.emission[2];
			}
			else
			{
				sample.x = mat.diffuse[0];
				sample.y = mat.diffuse[1];
				sample.z = mat.diffuse[2];

				sampleCounter++;
			}
		}
	}

	void
	MonteCarlo::GatherSampling(std::uint32_t pass) noexcept
	{
#pragma omp parallel for
		for (std::int32_t i = 0; i < this->renderData_.numEstimate; ++i)
		{
			if (!renderData_.rays[i].IsActive())
				continue;

			auto& hit = renderData_.hits[i];
			auto& sample = renderData_.samples[i];

			if (hit.shapeid != RadeonRays::kNullId && hit.primid != RadeonRays::kNullId)
			{
				tinyobj::mesh_t& mesh = scene_[hit.shapeid].mesh;
				tinyobj::material_t& mat = materials_[mesh.material_ids[hit.primid]];

				float atten = GetPhysicalLightAttenuation(renderData_.rays[i].o - ConvertFromBarycentric(mesh.positions.data(), mesh.indices.data(), hit.primid, hit.uvwt));
				if (mat.emission[0] > 0.0f || mat.emission[1] > 0.0f || mat.emission[2] > 0.0f)
				{
					sample.x *= mat.emission[0] * atten;
					sample.y *= mat.emission[1] * atten;
					sample.z *= mat.emission[2] * atten;
					sample *= renderData_.weights[i];
				}
				else
				{
					if (pass + 1 >= numBounces_)
						sample *= 0.0f;
					else
					{
						sample.x *= mat.diffuse[0] * atten;
						sample.y *= mat.diffuse[1] * atten;
						sample.z *= mat.diffuse[2] * atten;
						sample *= renderData_.weights[i];
					}
				}
			}
			else
			{
				sample *= skyColor_;
				sample *= renderData_.weights[i];
			}
		}
	}

	void
	MonteCarlo::GatherLightSamples() noexcept
	{
	}

	void
	MonteCarlo::Estimate(std::uint32_t frame, const RadeonRays::int2& offset, const RadeonRays::int2& size)
	{
		RadeonRays::Event* e = nullptr;
		RadeonRays::Intersection* hit = nullptr;

		this->GenerateWorkspace(size.x * size.y);
		this->GenerateNoise(frame, offset, size);

		this->GenerateFirstRays(frame, offset, size);

		for (std::int32_t pass = 0; pass < numBounces_; pass++)
		{
			api_->QueryIntersection(
				renderData_.fr_rays,
				renderData_.numEstimate,
				renderData_.fr_hits,
				nullptr,
				nullptr
			);

			this->GatherHits();

			if (pass == 0)
			{
				std::atomic_uint32_t count = 0;
				this->GatherFirstSampling(count);

				if (count == 0)
					break;
			}

			if (pass > 0)
			{
				this->GatherSampling(pass);
			}

			/*api_->QueryOcclusion(
				renderData_.fr_shadowrays,
				renderData_.numEstimate,
				renderData_.fr_shadowhits,
				nullptr,
				nullptr
			);

			this->GatherShadowHits();
			this->GatherLightSamples();*/

			// prepare ray for indirect lighting gathering
			this->GenerateRays(frame);
		}

		this->AccumSampling(frame, offset, size);
		this->AdaptiveSampling();

		this->ColorTonemapping(frame, offset, size);
	}

	void
	MonteCarlo::AccumSampling(std::uint32_t frame, const RadeonRays::int2& offset, const RadeonRays::int2& size) noexcept
	{
#pragma omp parallel for
		for (std::int32_t i = 0; i < size.x * size.y; ++i)
		{
			auto ix = offset.x + i % size.x;
			auto iy = offset.y + i / size.x;
			auto index = iy * this->width_ + ix;

			auto& hdr = hdr_[index];
			hdr.x += renderData_.samples[i].x;
			hdr.y += renderData_.samples[i].y;
			hdr.z += renderData_.samples[i].z;
		}
	}

	void
	MonteCarlo::AdaptiveSampling() noexcept
	{
	}

	void
	MonteCarlo::ColorTonemapping(std::uint32_t frame, const RadeonRays::int2& offset, const RadeonRays::int2& size) noexcept
	{
#pragma omp parallel for
		for (std::int32_t i = 0; i < size.x * size.y; ++i)
		{
			auto ix = offset.x + i % size.x;
			auto iy = offset.y + i / size.x;
			auto index = iy * this->width_ + ix;

			auto& hdr = hdr_[index];
			assert(std::isfinite(hdr.x));
			assert(std::isfinite(hdr.y));
			assert(std::isfinite(hdr.z));

			std::uint8_t r = TonemapACES(hdr.x / frame) * 255;
			std::uint8_t g = TonemapACES(hdr.y / frame) * 255;
			std::uint8_t b = TonemapACES(hdr.z / frame) * 255;

			ldr_[index] = 0xFF << 24 | b << 16 | g << 8 | r;
		}
	}

	void
	MonteCarlo::render(std::uint32_t frame, const RadeonRays::int2& offset, const RadeonRays::int2& size) noexcept
	{
		this->Estimate(frame, offset, size);
	}
}