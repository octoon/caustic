#include "MonteCarlo.h"
#include "Hammersley.h"
#include "BxDF.h"

#include <assert.h>

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
		, numBounces_(5)
		, numSamples_(1000)
		, skyColor_(1.0f, 1.0f, 1.0f)
		, light_(-0.01f, 1.9f, 0.1f)
		, width_(0)
		, height_(0)
		, api_(nullptr)
	{
	}

	MonteCarlo::MonteCarlo(std::uint32_t w, std::uint32_t h) noexcept
		: MonteCarlo()
	{
		this->setup(w, h);
	}

	MonteCarlo::~MonteCarlo() noexcept
	{
		RadeonRays::IntersectionApi::Delete(api_);
	}

	void
	MonteCarlo::setup(std::uint32_t w, std::uint32_t h) noexcept(false)
	{
		width_ = w;
		height_ = h;

		if (!init_data()) throw std::runtime_error("init_data() fail");
		if (!init_Gbuffers(w, h)) throw std::runtime_error("init_Gbuffers() fail");
		if (!init_RadeonRays()) throw std::runtime_error("init_RadeonRays() fail");
		if (!init_RadeonRays_Scene()) throw std::runtime_error("init_RadeonRays_Scene() fail");
	}

	bool
	MonteCarlo::init_Gbuffers(std::uint32_t, std::uint32_t h) noexcept
	{
		auto allocSize = width_ * height_;

		ldr_.resize(allocSize);
		hdr_.resize(allocSize);

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

		this->api_ = RadeonRays::IntersectionApi::Create(deviceidx);

		renderData_.rays.resize(this->width_);
		renderData_.hits.resize(this->width_);
		renderData_.samples.resize(this->width_);

		renderData_.fr_rays = api_->CreateBuffer(sizeof(RadeonRays::ray) * this->width_, nullptr);
		renderData_.fr_hits = api_->CreateBuffer(sizeof(RadeonRays::Intersection) * this->width_, nullptr);
		renderData_.fr_intersections = api_->CreateBuffer(sizeof(RadeonRays::Intersection) * this->width_, nullptr);

		return true;
	}

	bool
	MonteCarlo::init_data()
	{
		std::string basepath = "../Resources/CornellBox/";
		std::string filename = basepath + "orig.objm";
		std::string res = LoadObj(scene_, materials_, filename.c_str(), basepath.c_str());

		return res != "" ? false : true;
	}

	bool
	MonteCarlo::init_RadeonRays_Scene()
	{
		for (int id = 0; id < this->scene_.size(); ++id)
		{
			tinyobj::shape_t& objshape = this->scene_[id];

			float* vertdata = objshape.mesh.positions.data();
			int nvert = objshape.mesh.positions.size();
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
	MonteCarlo::raw_data(std::uint32_t y) const noexcept
	{
		return &ldr_[y * this->width_];
	}

	void
	MonteCarlo::GenerateFirstRays(std::uint32_t frame, std::uint32_t tile) noexcept
	{
		RadeonRays::ray* rays = nullptr;
		RadeonRays::Event* e = nullptr;
		api_->MapBuffer(renderData_.fr_rays, RadeonRays::kMapWrite, 0, sizeof(RadeonRays::ray) * this->width_, (void**)&rays, &e); e->Wait(); api_->DeleteEvent(e);

#pragma omp parallel for
		for (std::int32_t i = 0; i < this->width_; ++i)
		{
			const float xstep = 2.0f / (float)this->width_;
			const float ystep = 2.0f / (float)this->height_;
			float x = -1.f + xstep * (float)i;
			float y = ystep * (float)tile;
			float z = 1.f;

			rays[i].o = this->camera_;
			rays[i].d = RadeonRays::float3(x - this->camera_.x, y - this->camera_.y, z - this->camera_.z);
			rays[i].d.normalize();
			rays[i].SetMaxT(std::numeric_limits<float>::max());
			rays[i].SetTime(0.0f);
			rays[i].SetMask(-1);
			rays[i].SetActive(true);
			rays[i].SetDoBackfaceCulling(true);
		}

		std::memcpy(renderData_.rays.data(), rays, sizeof(RadeonRays::ray) * this->width_);

		api_->UnmapBuffer(renderData_.fr_rays, rays, &e); e->Wait(); api_->DeleteEvent(e);
	}

	void
	MonteCarlo::GenerateRays(std::uint32_t frame) noexcept
	{
		RadeonRays::ray* rays = nullptr;
		RadeonRays::Event* e = nullptr;
		api_->MapBuffer(renderData_.fr_rays, RadeonRays::kMapWrite, 0, sizeof(RadeonRays::ray) * this->width_, (void**)&rays, &e); e->Wait(); api_->DeleteEvent(e);

#pragma omp parallel for
		for (std::int32_t i = 0; i < this->width_; ++i)
		{
			auto& hit = renderData_.hits[i];
			if (hit.shapeid != RadeonRays::kNullId && hit.primid != RadeonRays::kNullId)
			{
				tinyobj::mesh_t& mesh = scene_[hit.shapeid].mesh;
				tinyobj::material_t& mat = materials_[mesh.material_ids[hit.primid]];

				if (mat.emission[0] == 0.0f && mat.emission[1] == 0.0f && mat.emission[2] == 0.0f)
				{
					auto ior = mat.dissolve;
					auto roughness = mat.shininess;
					auto ro = ConvertFromBarycentric(mesh.positions.data(), mesh.indices.data(), hit.primid, hit.uvwt);
					auto norm = ConvertFromBarycentric(mesh.normals.data(), mesh.indices.data(), hit.primid, hit.uvwt);

					RadeonRays::float3 L = bsdf(renderData_.rays[i].d, norm, roughness, ior, frame, numSamples_);
					renderData_.rays[i].d = L;
					renderData_.rays[i].o = ro + L * 1e-4f;
					renderData_.rays[i].SetMaxT(std::numeric_limits<float>::max());
					renderData_.rays[i].SetTime(0.0f);
					renderData_.rays[i].SetMask(-1);
					renderData_.rays[i].SetActive(RadeonRays::dot(norm, L) > 0.0 ? true : false);
					renderData_.rays[i].SetDoBackfaceCulling(ior > 1.0f ? false : true);
				}
				else
				{
					renderData_.rays[i].SetActive(false);
				}
			}
			else
			{
				renderData_.rays[i].SetActive(false);
			}
		}

		std::memcpy(rays, renderData_.rays.data(), sizeof(RadeonRays::ray) * renderData_.rays.size());

		api_->UnmapBuffer(renderData_.fr_rays, rays, &e); e->Wait(); api_->DeleteEvent(e);
	}

	void
	MonteCarlo::GatherHits(std::uint32_t frame, std::uint32_t tile) noexcept
	{
		RadeonRays::Event* e = nullptr;
		RadeonRays::Intersection* hit = nullptr;

		api_->QueryIntersection(renderData_.fr_rays, renderData_.rays.size(), renderData_.fr_hits, nullptr, nullptr);
		api_->MapBuffer(renderData_.fr_hits, RadeonRays::kMapRead, 0, sizeof(RadeonRays::Intersection) * this->width_, (void**)&hit, &e); e->Wait(); api_->DeleteEvent(e);

		std::memcpy(renderData_.hits.data(), hit, sizeof(RadeonRays::Intersection) * this->width_);

		api_->UnmapBuffer(renderData_.fr_hits, hit, &e); e->Wait(); api_->DeleteEvent(e);
	}

	void
	MonteCarlo::GatherFirstSampling(std::uint32_t tile) noexcept
	{
#pragma omp parallel for
		for (std::int32_t i = 0; i < this->width_; ++i)
		{
			auto& hit = renderData_.hits[i];
			auto& sample = renderData_.samples[i];

			int shape_id = hit.shapeid;
			int prim_id = hit.primid;

			if (shape_id == RadeonRays::kNullId || prim_id == RadeonRays::kNullId)
				continue;

			tinyobj::mesh_t& mesh = scene_[shape_id].mesh;
			tinyobj::material_t& mat = materials_[mesh.material_ids[prim_id]];

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
			}
		}
	}

	void
	MonteCarlo::GatherSampling(std::uint32_t pass, std::uint32_t tile) noexcept
	{
#pragma omp parallel for
		for (std::int32_t i = 0; i < this->width_; ++i)
		{
			auto& hit = renderData_.hits[i];
			auto& sample = renderData_.samples[i];

			if (hit.shapeid != RadeonRays::kNullId && hit.primid != RadeonRays::kNullId)
			{
				tinyobj::mesh_t& mesh = scene_[hit.shapeid].mesh;
				tinyobj::material_t& mat = materials_[mesh.material_ids[hit.primid]];

				if (mat.emission[0] > 0.0f || mat.emission[1] > 0.0f || mat.emission[2] > 0.0f)
				{
					sample.x *= mat.emission[0];
					sample.y *= mat.emission[1];
					sample.z *= mat.emission[2];
				}
				else
				{
					if (pass + 1 >= numBounces_)
						sample *= 0.0f;
					else
					{
						sample.x *= mat.diffuse[0];// * renderData_.rays[i].d.w;
						sample.y *= mat.diffuse[1];// * renderData_.rays[i].d.w;
						sample.z *= mat.diffuse[2];// * renderData_.rays[i].d.w;
					}
				}
			}
			else
			{
				sample *= skyColor_;
			}
		}
	}

	void
	MonteCarlo::Estimate(std::uint32_t frame, std::uint32_t tile)
	{
		RadeonRays::Event* e = nullptr;
		RadeonRays::Intersection* hit = nullptr;

		this->GenerateFirstRays(frame, tile);
		this->GatherHits(frame, tile);
		this->GatherFirstSampling(tile);

		for (std::int32_t pass = 0; pass < numBounces_; pass++)
		{
			// prepare ray for indirect lighting gathering
			this->GenerateRays(frame);
			this->GatherHits(frame, tile);
			this->GatherSampling(pass, tile);
		}

		this->AccumSampling(frame, tile);
	}

	void
	MonteCarlo::AccumSampling(std::uint32_t frame, std::uint32_t tile) noexcept
	{
#pragma omp parallel for
		for (std::int32_t i = 0; i < this->width_; ++i)
		{
			auto index = tile * this->width_ + i;

			auto& hdr = hdr_[index];
			hdr.x += renderData_.samples[i].x;
			hdr.y += renderData_.samples[i].y;
			hdr.z += renderData_.samples[i].z;

			std::uint8_t r = TonemapACES(hdr.x / frame) * 255;
			std::uint8_t g = TonemapACES(hdr.y / frame) * 255;
			std::uint8_t b = TonemapACES(hdr.z / frame) * 255;

			ldr_[index] = 0xFF << 24 | b << 16 | g << 8 | r;
		}
	}

	void
	MonteCarlo::render(std::uint32_t tile, std::uint32_t frame) noexcept
	{
		this->Estimate(frame, tile);
	}
}