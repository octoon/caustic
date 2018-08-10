#include "disney.h"
#include "math.h"
#include <assert.h>

namespace octoon
{
	namespace caustic
	{
		inline float luminance(const RadeonRays::float3& rgb)
		{
			return RadeonRays::dot(rgb, RadeonRays::float3(0.299f, 0.587f, 0.114f));
		}

		inline RadeonRays::float4 UniformSampleSphere(const RadeonRays::float2& Xi)
		{
			float phi = 2 * PI * Xi.x;

			float cosTheta = 1 - 2 * Xi.y;
			float sinTheta = fast_sqrt(1 - cosTheta * cosTheta);

			RadeonRays::float4 H;
			H.x = fast_cos(phi) * sinTheta;
			H.y = fast_sin(phi) * sinTheta;
			H.z = cosTheta;
			H.w = 1.0 / (4 * PI);

			return H;
		}

		inline RadeonRays::float3 UniformSampleHemisphere(const RadeonRays::float2& Xi)
		{
			float phi = Xi.x * 2 * PI;

			float cosTheta = Xi.y;
			float sinTheta = fast_sqrt(1.0f - cosTheta * cosTheta);

			RadeonRays::float3 H;
			H.x = fast_cos(phi) * sinTheta;
			H.y = fast_sin(phi) * sinTheta;
			H.z = cosTheta;
			H.w = 1.0f / (2 * PI);

			return H;
		}

		inline RadeonRays::float3 UniformSampleCone(const RadeonRays::float2& Xi, float CosThetaMax)
		{
			float phi = 2 * PI * Xi.x;
			float cosTheta = CosThetaMax * (1 - Xi.y) + Xi.y;
			float sinTheta = fast_sqrt(1 - cosTheta * cosTheta);

			RadeonRays::float3 H;
			H.x = sinTheta * fast_cos(phi);
			H.y = sinTheta * fast_sin(phi);
			H.z = cosTheta;
			H.w = 1.0f / (2 * PI * (1 - CosThetaMax));

			return H;
		}

		inline RadeonRays::float3 CosineSampleHemisphere(const RadeonRays::float2& Xi)
		{
			float phi = Xi.x * 2.0f * PI;

			float cosTheta = fast_sqrt(Xi.y);
			float sinTheta = fast_sqrt(1.0f - cosTheta * cosTheta);

			RadeonRays::float3 H;
			H.x = fast_cos(phi) * sinTheta;
			H.y = fast_sin(phi) * sinTheta;
			H.z = cosTheta;
			H.w = cosTheta * (1 / PI);

			return H;
		}

		inline RadeonRays::float3 ImportanceSampleGGX(const RadeonRays::float2& Xi, float roughness)
		{
			float m = roughness * roughness;
			float m2 = m * m;
			float u = (1.0f - Xi.y) / (1.0f + (m2 - 1) * Xi.y);

			return CosineSampleHemisphere(RadeonRays::float2(Xi.x, u));
		}

		inline RadeonRays::float3 ImportanceSampleBlinn(const RadeonRays::float2& Xi, float a2)
		{
			float phi = Xi.x * 2.0f * PI;

			float n = 2 / a2 - 2;
			float cosTheta = std::pow(Xi.y, 1 / (n + 1));
			float sinTheta = fast_sqrt(1 - cosTheta * cosTheta);

			RadeonRays::float3 H;
			H.x = sinTheta * fast_cos(phi);
			H.y = sinTheta * fast_sin(phi);
			H.z = cosTheta;

			return H;
		}

		RadeonRays::float3 DiffuseBRDF(const RadeonRays::float3& N, const RadeonRays::float3& L, const RadeonRays::float3& V, float roughness)
		{
			float nl = RadeonRays::dot(L, N);
			if (nl > 0)
			{
				auto H = RadeonRays::normalize(L + V);

				float vh = std::max(0.0f, RadeonRays::dot(V, L));
				float nv = std::max(0.1f, RadeonRays::dot(V, N));

				float Fd90 = (0.5f + 2 * vh * vh) * roughness;
				float FdV = lerp(1, Fd90, pow5(1 - nv));
				float FdL = lerp(1, Fd90, pow5(1 - nl));

				// lambert = DiffuseColor * NoL / PI
				// pdf = NoL / PI
				float fresnel = FdV * FdL * lerp(1.0f, 1.0f / 1.51, roughness);

				return RadeonRays::float3(fresnel, fresnel, fresnel);
			}

			return 0;
		}

		RadeonRays::float3 SpecularBRDF_GGX(const RadeonRays::float3& N, const RadeonRays::float3& L, const RadeonRays::float3& V, const RadeonRays::float3& f0, float roughness)
		{
			float nl = RadeonRays::dot(L, N);
			if (nl > 0)
			{
				auto H = RadeonRays::normalize(L + V);

				float nv = saturate(RadeonRays::dot(N, V));
				float vh = saturate(RadeonRays::dot(V, H));
				float nh = saturate(RadeonRays::dot(N, H));

				float m = roughness * roughness;
				float Gv = nl * (nv * (1 - m) + m);
				float Gl = nv * (nl * (1 - m) + m);
				float G = 0.5f / (Gv + Gl);

				float Fc = pow5(1 - vh);
				RadeonRays::float3 F = f0 * (1 - Fc) + RadeonRays::float3(Fc, Fc, Fc);

				// Incident light = SampleColor * NoL
				// Microfacet specular = D*G*F / (4*NoL*NoV) = D*Vis*F
				// pdf = D * NoH / (4 * VoH)
				return F * G * nl * (4 * vh / nh);
			}

			return 0;
		}

		RadeonRays::float3 SpecularBTDF_GGX(const RadeonRays::float3& N, const RadeonRays::float3& L, const RadeonRays::float3& V, const RadeonRays::float3& f0, float roughness)
		{
			float nl = saturate(RadeonRays::dot(L, N));
			if (nl > 0)
			{
				auto H = RadeonRays::normalize(L + V);

				float nv = saturate(RadeonRays::dot(N, V));
				float vh = saturate(RadeonRays::dot(V, H));
				float nh = saturate(RadeonRays::dot(N, H));

				float m = roughness * roughness;
				float Gv = nl * (nv * (1 - m) + m);
				float Gl = nv * (nl * (1 - m) + m);
				float G = 0.5f / (Gv + Gl);

				float Fc = pow5(1 - nv);
				RadeonRays::float3 Fr = f0 * (1 - Fc) + RadeonRays::float3(Fc, Fc, Fc);
				RadeonRays::float3 Ft = RadeonRays::float3(1.0f, 1.0f, 1.0f) - Fr;

				return Ft * G * nl * (4 * vh / nh);
			}

			return 0;
		}

		RadeonRays::float3 TangentToWorld(const RadeonRays::float3& H, const RadeonRays::float3& N)
		{
			RadeonRays::float3 Y = std::abs(N.z) < 0.999f ? RadeonRays::float3(0, 0, 1) : RadeonRays::float3(1, 0, 0);
			RadeonRays::float3 X = RadeonRays::normalize(RadeonRays::cross(Y, N));
			return RadeonRays::normalize(X * H.x + RadeonRays::cross(N, X) * H.y + N * H.z);
		}

		RadeonRays::float3 LobeDirection(const RadeonRays::float3& n, float roughness, const RadeonRays::float2& Xi)
		{
			auto H = ImportanceSampleGGX(Xi, roughness);
			return TangentToWorld(H, n);
		}

		RadeonRays::float3 CosineDirection(const RadeonRays::float3& n, const RadeonRays::float2& Xi)
		{
			auto H = UniformSampleHemisphere(Xi);
			return TangentToWorld(H, n);
		}

		RadeonRays::float3 Disney_Evaluate(const RadeonRays::float3& N, const RadeonRays::float3& V, const RadeonRays::float3& L, const Material& mat, const RadeonRays::float2& sample) noexcept
		{
			if (mat.ior > 1.0f)
			{
				auto f0 = RadeonRays::float3(mat.specular[0], mat.specular[1], mat.specular[2]);
				f0.x = lerp(f0.x, mat.albedo.x, mat.metalness);
				f0.y = lerp(f0.y, mat.albedo.y, mat.metalness);
				f0.z = lerp(f0.z, mat.albedo.z, mat.metalness);

				return SpecularBTDF_GGX(N, -L, N, f0, mat.roughness) * mat.albedo;
			}
			else
			{
				float cd_lum = luminance(mat.albedo);
				float cs_lum = luminance(mat.specular);
				float cs_w = cs_lum / (cs_lum + (1.f - mat.metalness) * cd_lum);

				auto E = sample;
				if (E.y <= cs_w)
				{
					E.y /= cs_w;

					auto f0 = RadeonRays::float3(mat.specular[0], mat.specular[1], mat.specular[2]);
					f0.x = lerp(f0.x, mat.albedo.x, mat.metalness);
					f0.y = lerp(f0.y, mat.albedo.y, mat.metalness);
					f0.z = lerp(f0.z, mat.albedo.z, mat.metalness);

					return SpecularBRDF_GGX(N, L, V, f0, mat.roughness);
				}
				else
				{
					E.y -= cs_w;
					E.y /= (1.0f - cs_w);

					return DiffuseBRDF(N, L, V, mat.roughness) * mat.albedo * (1.0f - mat.metalness);
				}
			}
		}

		RadeonRays::float3 Disney_Sample(const RadeonRays::float3& N, const RadeonRays::float3& V, const Material& mat, const RadeonRays::float2& sample) noexcept
		{
			if (mat.ior > 1.0f)
			{
				return refract(-V, LobeDirection(N, mat.roughness, sample), 1.0f / mat.ior);
			}
			else
			{
				float cd_lum = RadeonRays::dot(mat.albedo, RadeonRays::float3(0.3f, 0.6f, 0.1f));
				float cs_lum = RadeonRays::dot(mat.specular, RadeonRays::float3(0.3f, 0.6f, 0.1f));
				float cs_w = cs_lum / (cs_lum + (1.f - mat.metalness) * cd_lum);

				auto E = sample;
				if (E.y <= cs_w)
				{
					E.y /= cs_w;

					return reflect(-V, LobeDirection(N, mat.roughness, E));
				}
				else
				{
					E.y -= cs_w;
					E.y /= (1.0f - cs_w);

					return CosineDirection(N, E);
				}
			}
		}
	}
}