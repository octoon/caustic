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
		, numBounces_(1)
		, numSamples_(100)
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
		if (!init_RadeonRays_Camera()) throw std::runtime_error("init_RadeonRays_Camera() fail");
	}

	bool 
	MonteCarlo::init_Gbuffers(std::uint32_t, std::uint32_t h) noexcept
	{
		auto allocSize = width_ * height_;

		ldr_.resize(allocSize);
		hdr_.resize(allocSize);

		hits_.resize(allocSize);
		albede_.resize(allocSize);
		normals_.resize(allocSize);
		position_.resize(allocSize);

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
		return true;
	}

	bool
	MonteCarlo::init_RadeonRays_Camera()
	{
		const int numRays = this->width_ * this->height_;

		// Prepare rays. One for each texture pixel.
		std::vector<RadeonRays::ray> rays(numRays);

#pragma omp parallel for
		for (std::int32_t i = 0; i < this->height_; ++i)
		{
			for (std::int32_t j = 0; j < this->width_; ++j)
			{
				const float xstep = 2.f / (float)this->width_;
				const float ystep = 2.f / (float)this->height_;
				float x = -1.f + xstep * (float)j;
				float y = ystep * (float)i;
				float z = 1.f;

				rays[i * this->width_ + j].o = this->camera_;
				rays[i * this->width_ + j].d = RadeonRays::float3(x - this->camera_.x, y - this->camera_.y, z - this->camera_.z);
				rays[i * this->width_ + j].d.normalize();
			}
		}

		view_ = std::move(rays);

		this->ray_ = api_->CreateBuffer(sizeof(RadeonRays::ray), nullptr);
		this->hit_ = api_->CreateBuffer(sizeof(RadeonRays::Intersection), nullptr);
		this->ray_buffer_ = api_->CreateBuffer(sizeof(RadeonRays::ray) * numRays, view_.data());
		this->isect_buffer_ = api_->CreateBuffer(sizeof(RadeonRays::Intersection) * numRays, nullptr);

		return true;
	}

	bool 
	MonteCarlo::init_data()
	{
		std::string basepath = "../Resources/CornellBox/";
		std::string filename = basepath + "orig.objm";
		std::string res = LoadObj(g_objshapes, g_objmaterials, filename.c_str(), basepath.c_str());

		return res != "" ? false : true;
	}

	bool 
	MonteCarlo::init_RadeonRays_Scene()
	{
		for (int id = 0; id < this->g_objshapes.size(); ++id)
		{
			tinyobj::shape_t& objshape = this->g_objshapes[id];

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

	RadeonRays::float3
	MonteCarlo::PathTracing(RadeonRays::float3 ro, RadeonRays::float3 rd, RadeonRays::float3 norm, float roughness, float ior, std::uint32_t frame)
	{
		RadeonRays::ray* rays = nullptr;
		RadeonRays::Event* e = nullptr;
		RadeonRays::Intersection* hit = nullptr;
		RadeonRays::float3 colorAccum(1.0f, 1.0f, 1.0f);
		RadeonRays::float3 finalColor(0.0f, 0.0f, 0.0f);

		for (std::int32_t i = 0; i < numBounces_; i++)
		{
			auto d = bsdf(rd, norm, roughness, ior, frame, numSamples_);

			api_->MapBuffer(this->ray_, RadeonRays::kMapWrite, 0, sizeof(RadeonRays::ray), (void**)&rays, &e); e->Wait(); api_->DeleteEvent(e);
			rays[0].d = d;
			rays[0].o = ro + rays[0].d * 1e-4f;
			rays[0].SetMaxT(std::numeric_limits<float>::max());
			rays[0].SetTime(0.0f);
			rays[0].SetMask(-1);
			rays[0].SetActive(true);
			rays[0].SetDoBackfaceCulling(true);

			api_->UnmapBuffer(this->ray_, rays, &e); e->Wait(); api_->DeleteEvent(e);
			api_->QueryIntersection(this->ray_, 1, this->hit_, nullptr, &e); e->Wait(); api_->DeleteEvent(e);
			api_->MapBuffer(this->hit_, RadeonRays::kMapRead, 0, sizeof(RadeonRays::Intersection), (void**)&hit, &e); e->Wait(); api_->DeleteEvent(e);

			if (hit[0].shapeid != RadeonRays::kNullId && hit[0].primid != RadeonRays::kNullId)
			{
				tinyobj::mesh_t& mesh = g_objshapes[hit[0].shapeid].mesh;
				tinyobj::material_t& mat = g_objmaterials[mesh.material_ids[hit[0].primid]];

				if (mat.emission[0] > 0.0f || mat.emission[1] > 0.0f || mat.emission[2] > 0.0f)
				{
					finalColor = colorAccum * RadeonRays::float3(mat.emission[0], mat.emission[1], mat.emission[2]);
					break;
				}
				else
				{
					auto albede = RadeonRays::float3(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]);
					auto p = ConvertFromBarycentric(mesh.positions.data(), mesh.indices.data(), hit[0].primid, hit[0].uvwt);
					norm = ConvertFromBarycentric(mesh.normals.data(), mesh.indices.data(), hit[0].primid, hit[0].uvwt);
					rd = d;
					ior = mat.dissolve;
					roughness = mat.shininess;

					colorAccum *= albede * GetPhysicalLightAttenuation(ro - p);

					ro = p;
				}
			}
			else
			{
				finalColor = colorAccum * skyColor_;
				break;
			}
			
			api_->UnmapBuffer(this->hit_, hit, nullptr);
		}

		return finalColor;
	}

	RadeonRays::float3
	MonteCarlo::MultPathTracing(const RadeonRays::float3& ro, const RadeonRays::float3& rd, const RadeonRays::float3& norm, float roughness, float ior, std::uint32_t bounce)
	{
		if (bounce > this->numBounces_)
			return RadeonRays::float3(0.0f, 0.0f, 0.0f);

		bounce++;

		RadeonRays::ray* rays = nullptr;
		RadeonRays::Event* e = nullptr;
		RadeonRays::Intersection* hits = nullptr;
		RadeonRays::float3 colorAccum(0.0f, 0.0f, 0.0f);

		auto spp_ray = api_->CreateBuffer(sizeof(RadeonRays::ray) * numSamples_, nullptr);
		auto spp_hit = api_->CreateBuffer(sizeof(RadeonRays::Intersection) * numSamples_, nullptr);

		api_->MapBuffer(spp_ray, RadeonRays::kMapWrite, 0, sizeof(RadeonRays::ray) * numSamples_, (void**)&rays, &e); e->Wait(); api_->DeleteEvent(e);

#pragma omp parallel for
		for (auto i = 0; i < numSamples_; i++)
		{
			RadeonRays::float3 L = bsdf(rd, norm, roughness, ior, i, numSamples_);

			auto& ray = rays[i];
			ray.o = ro + L * 1e-4f;
			ray.d = L;
			ray.SetMaxT(std::numeric_limits<float>::max());
			ray.SetTime(0.0f);
			ray.SetMask(-1);
			ray.SetActive(true);
			ray.SetDoBackfaceCulling(ior > 1.0f ? false : true);
		}

		api_->UnmapBuffer(spp_ray, rays, &e); e->Wait(); api_->DeleteEvent(e);
		api_->QueryIntersection(spp_ray, numSamples_, spp_hit, nullptr, &e); e->Wait(); api_->DeleteEvent(e);
		api_->MapBuffer(spp_hit, RadeonRays::kMapRead, 0, sizeof(RadeonRays::Intersection) * numSamples_, (void**)&hits, &e); e->Wait(); api_->DeleteEvent(e);

		std::vector<std::uint8_t> inects(numSamples_, false);
		std::vector<RadeonRays::float3> albede(numSamples_);
		std::vector<RadeonRays::float3> normals(numSamples_);
		std::vector<RadeonRays::float3> position(numSamples_);

#pragma omp parallel for
		for (std::int32_t i = 0; i < numSamples_; i++)
		{
			auto& hit = hits[i];
			if (hit.shapeid != RadeonRays::kNullId && hit.primid != RadeonRays::kNullId)
			{
				tinyobj::mesh_t& mesh = g_objshapes[hit.shapeid].mesh;
				tinyobj::material_t& mat = g_objmaterials[mesh.material_ids[hit.primid]];

				if (mat.emission[0] > 0.0f || mat.emission[1] > 0.0f || mat.emission[2] > 0.0f)
					colorAccum += RadeonRays::float3(mat.emission[0], mat.emission[1], mat.emission[2]);
				else
				{
					inects[i] = true;
					albede[i] = RadeonRays::float3(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]);
					position[i] = ConvertFromBarycentric(mesh.positions.data(), mesh.indices.data(), hit.primid, hit.uvwt);
					position_[i].w = mat.dissolve;
					normals[i] = ConvertFromBarycentric(mesh.normals.data(), mesh.indices.data(), hit.primid, hit.uvwt);
					normals_[i].w = mat.shininess;
				}
			}
			else
			{
				colorAccum += skyColor_;
			}
		}

		api_->UnmapBuffer(spp_hit, hits, nullptr);

		api_->DeleteBuffer(spp_ray);
		api_->DeleteBuffer(spp_hit);

		for (std::size_t i = 0; i < numSamples_; i++)
		{
			if (inects[i])
				colorAccum += albede[i] * MultPathTracing(position[i], rays[i].d, normals[i], normals_[i].w, position[i].w, bounce) * (1.0f / numSamples_) * GetPhysicalLightAttenuation(position[i] - ro);
		}

		return colorAccum * (1.0f / numSamples_);
	}

	void 
	MonteCarlo::query() noexcept
	{
		RadeonRays::Event* e = nullptr;
		RadeonRays::Intersection* isect = nullptr;

		api_->QueryIntersection(ray_buffer_, this->width_ * this->height_, this->isect_buffer_, nullptr, nullptr);
		api_->MapBuffer(this->isect_buffer_, RadeonRays::kMapRead, 0, this->width_ * this->height_ * sizeof(RadeonRays::Intersection), (void**)&isect, &e); e->Wait(); api_->DeleteEvent(e);

#pragma omp parallel for
		for (std::int32_t i = 0; i < this->width_ * this->height_; ++i)
		{
			int shape_id = isect[i].shapeid;
			int prim_id = isect[i].primid;

			if (shape_id == RadeonRays::kNullId || prim_id == RadeonRays::kNullId)
				continue;

			tinyobj::mesh_t& mesh = g_objshapes[shape_id].mesh;
			tinyobj::material_t& mat = g_objmaterials[mesh.material_ids[prim_id]];

			if (mat.emission[0] > 0.0f || mat.emission[1] > 0.0f || mat.emission[2] > 0.0f)
			{
				hits_[i] = false;
				hdr_[i] += RadeonRays::float3(mat.emission[0], mat.emission[1], mat.emission[2]);
			}
			else
			{
				hits_[i] = true;
				normals_[i] = ConvertFromBarycentric(mesh.normals.data(), mesh.indices.data(), prim_id, isect[i].uvwt);
				normals_[i].w = mat.shininess;
				position_[i] = ConvertFromBarycentric(mesh.positions.data(), mesh.indices.data(), prim_id, isect[i].uvwt);
				position_[i].w = mat.dissolve;
				albede_[i] = RadeonRays::float3(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]);
			}
		}
	}

	void
	MonteCarlo::render(std::uint32_t y, std::uint32_t frame) noexcept
	{
		for (std::uint32_t i = y * this->width_; i < y * this->width_ + this->width_; ++i)
		{
			if (hits_[i] > 0)
			{
				std::uint32_t bounce = 0;
				if (numSamples_)
					hdr_[i] += albede_[i] * MultPathTracing(position_[i], view_[i].d, normals_[i], normals_[i].w, position_[i].w, bounce);
				else
					hdr_[i] += albede_[i] * PathTracing(position_[i], view_[i].d, normals_[i], normals_[i].w, position_[i].w, frame);
			}
		}

#pragma omp parallel for
		for (std::int32_t i = y * this->width_; i < y * this->width_ + this->width_; ++i)
		{
			std::uint8_t r = TonemapACES(hdr_[i].x / frame) * 255;
			std::uint8_t g = TonemapACES(hdr_[i].y / frame) * 255;
			std::uint8_t b = TonemapACES(hdr_[i].z / frame) * 255;

			ldr_[i] = 0xFF << 24 | b << 16 | g << 8 | r;
		}
	}
}