#ifndef OCTOON_CAUSTIC_LIGHT_H_
#define OCTOON_CAUSTIC_LIGHT_H_

#include <octoon/caustic/object.h>
#include <octoon/caustic/material.h>

namespace octoon
{
	namespace caustic
	{
		class Light : public Object
		{
		public:
			Light() noexcept;
			virtual ~Light() noexcept;

			void setColor(const RadeonRays::float3& color) noexcept;
			const RadeonRays::float3& getColor() const noexcept;

			virtual RadeonRays::float4 sample(const RadeonRays::float3& P, const RadeonRays::float3& N, const Material& mat, const RadeonRays::float2& Xi) const noexcept;
			virtual RadeonRays::float3 Li(const RadeonRays::float3& N, const RadeonRays::float3& V, const RadeonRays::float3& L, const Material& mat, const RadeonRays::float2& Xi) const noexcept;

		private:
			Light(const Light&) noexcept = delete;
			Light& operator=(const Light&) noexcept = delete;

		private:
			RadeonRays::float3 color_;
		};
	}
}

#endif