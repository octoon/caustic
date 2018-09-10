#include "disney.h"
#include "math.h"
#include <assert.h>

namespace octoon
{
	namespace caustic
	{
		float SmithG_GGX(float nv, float alpha)
		{
			float a = alpha * alpha;
			float b = nv * nv;
			return 1 / (nv + std::sqrt(a + b - a * b));
		}

		float SmithGGX_Correlated(float nl, float nv, float alpha)
		{
			// Height correlated Smith GGX geometry term as defined in (equation 3 of section 3.1.2):
			// Course notes "Moving Frostbite to PBR" by DICE
			// http://www.frostbite.com/wp-content/uploads/2014/11/course_notes_moving_frostbite_to_pbr.pdf
			float alpha2 = alpha * alpha;
			float nl2 = nl * nl;
			float nv2 = nv * nv;

			// Original formulation of G_SmithGGX Correlated
			float Gv = (-1 + sqrt(1 + alpha2 * (1 - nv2) / nv2)) / 2.0f;
			float Gl = (-1 + sqrt(1 + alpha2 * (1 - nl2) / nl2)) / 2.0f;
			float G = 1 / (1 + Gv + Gl);

			return G;
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
				float F = FdV * FdL * lerp(1.0f, 1.0f / 1.51, roughness);

				RadeonRays::float4 brdf;
				brdf.x = F * nl / PI;
				brdf.y = F * nl / PI;
				brdf.z = F * nl / PI;
				brdf.w = nl / PI;

				return brdf;
			}

			return RadeonRays::float4(0, 0, 0, 1);
		}

		RadeonRays::float4 SpecularBRDF_GGX(const RadeonRays::float3& N, const RadeonRays::float3& L, const RadeonRays::float3& V, const RadeonRays::float3& f0, float roughness)
		{
			float nl = RadeonRays::dot(L, N);
			float nv = RadeonRays::dot(N, V);

			if (nl > 0 && nv > 0)
			{
				auto H = RadeonRays::normalize(L + V);

				float vh = saturate(RadeonRays::dot(V, H));
				float nh = saturate(RadeonRays::dot(N, H));

				float m = roughness * roughness;
				float m2 = m * m;
				float spec = (nh * m2 - nh) * nh + 1;
				float D = m2 / (spec * spec);

				float Gv = nl * (nv * (1 - m) + m);
				float Gl = nv * (nl * (1 - m) + m);
				float G = 0.5f / (Gv + Gl);

				float Fc = pow5(1 - vh);
				RadeonRays::float3 F = f0 * (1 - Fc) + RadeonRays::float3(Fc, Fc, Fc);

				// Incident light = SampleColor * NoL
				// Microfacet specular = D*G*F / (4*NoL*NoV) = D*Vis*F
				// pdf = D * NoH / (4 * VoH)
				RadeonRays::float4 brdf;
				brdf.x = F.x * D * G * nl;
				brdf.y = F.y * D * G * nl;
				brdf.z = F.z * D * G * nl;
				brdf.w = D * nh / (4 * vh);

				return brdf;
			}

			return RadeonRays::float4(0, 0, 0, 1);
		}

		RadeonRays::float4 SpecularBTDF_GGX(const RadeonRays::float3& N, const RadeonRays::float3& L, const RadeonRays::float3& V, const RadeonRays::float3& f0, float roughness, float ior)
		{
			float nl = RadeonRays::dot(N, L);
			float nv = RadeonRays::dot(N, V);

			if (nl > 0 && nv > 0)
			{
				auto H = RadeonRays::normalize(L + V);

				float vh = saturate(RadeonRays::dot(V, H));
				float nh = saturate(RadeonRays::dot(N, H));
				float lh = saturate(RadeonRays::dot(L, H));

				float m = roughness * roughness;
				float m2 = m * m;
				float spec = (nh * m2 - nh) * nh + 1;
				float D = m2 / (spec * spec);

				float Gv = nl * (nv * (1 - m) + m);
				float Gl = nv * (nl * (1 - m) + m);
				float G = 0.5f / (Gv + Gl) * (4 * nl * nv);

				float Fc = pow5(1 - lh);
				RadeonRays::float3 F = f0 * (1 - Fc) + RadeonRays::float3(Fc, Fc, Fc);

				// Refraction term according to equation 21 of:
				// https://www.cs.cornell.edu/~srm/publications/EGSR07-btdf.pdf
				float ni = ior;
				float no = ior;
				float no2 = ior * ior;
				float A = (lh * vh) / (nl * nv);
				float B = (ni * lh + no * vh);
				float B2 = B * B;

				RadeonRays::float4 btdf;
				btdf.x = A * no2 * (1.0f - F.x) * D * G * nl / B2;
				btdf.y = A * no2 * (1.0f - F.y) * D * G * nl / B2;
				btdf.z = A * no2 * (1.0f - F.z) * D * G * nl / B2;
				btdf.w = A * no2 * D / B2;

				return btdf;
			}

			return RadeonRays::float4(0, 0, 0, 1);
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

		RadeonRays::float4 Disney_Evaluate(const RadeonRays::float3& N, const RadeonRays::float3& wi, const RadeonRays::float3& wo, const Material& mat, const RadeonRays::float2& sample) noexcept
		{
			float cd_lum = luminance(mat.albedo);
			float cs_lum = luminance(mat.specular);
			float cs_w = cs_lum / (cs_lum + (1.f - mat.metalness) * cd_lum);

			auto E = sample;
			if (E.y <= cs_w)
			{
				auto f0 = RadeonRays::float3(mat.specular[0], mat.specular[1], mat.specular[2]);
				f0.x = lerp(f0.x, mat.albedo.x, mat.metalness);
				f0.y = lerp(f0.y, mat.albedo.y, mat.metalness);
				f0.z = lerp(f0.z, mat.albedo.z, mat.metalness);

				return SpecularBRDF_GGX(N, wo, wi, f0, mat.roughness);
			}
			else
			{
				if (mat.ior > 1.0f)
				{
					auto f0 = RadeonRays::float3(mat.specular[0], mat.specular[1], mat.specular[2]);
					f0.x = lerp(f0.x, mat.albedo.x, mat.metalness);
					f0.y = lerp(f0.y, mat.albedo.y, mat.metalness);
					f0.z = lerp(f0.z, mat.albedo.z, mat.metalness);

					return SpecularBTDF_GGX(N, -wo, wi, f0, mat.roughness, mat.ior);
				}
				else
				{
					return DiffuseBRDF(N, wo, wi, mat.roughness) * mat.albedo * (1.0f - mat.metalness);
				}
			}
		}

		RadeonRays::float3 Disney_Sample(const RadeonRays::float3& norm, const RadeonRays::float3& wi, const Material& mat, const RadeonRays::float2& sample, RadeonRays::float4& wo) noexcept
		{
			float cd_lum = luminance(mat.albedo);
			float cs_lum = luminance(mat.specular);
			float cs_w = cs_lum / (cs_lum + (1.f - mat.metalness) * cd_lum);

			auto N = norm;
			auto E = sample;
			if (E.y <= cs_w)
			{
				E.y /= cs_w;

				wo = LobeDirection(reflect(-wi, N), mat.roughness, E);
			}
			else
			{
				E.y -= cs_w;
				E.y /= (1.0f - cs_w);

				if (mat.ior > 1.0f)
				{
					if (RadeonRays::dot(wi, N) < 0.0f)
						N = -N;

					wo = RadeonRays::normalize(refract(-wi, LobeDirection(N, mat.roughness, E), 1.0f / mat.ior));
				}
				else
				{
					wo = CosineDirection(N, E);
				}
			}

			return Disney_Evaluate(N, wi, wo, mat, sample);
		}
	}
}