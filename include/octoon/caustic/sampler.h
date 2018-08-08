#ifndef OCTOON_CAUSTIC_SAMPLER
#define OCTOON_CAUSTIC_SAMPLER

#include <algorithm>
#include <octoon/caustic/texture.h>

namespace octoon
{
    namespace caustic
    {
        template<class T>
        class Sampler
        {
        public:
			enum class Wrap : std::uint8_t
			{
				Repeat = 1,
				Mirror = 2,
				ClampToEdge = 3,
			};

			enum class Filter : std::uint8_t
			{
				Nearest = 0,
				Linear = 1,
				NearestMipmapLinear = 2,
				NearestMipmapNearest = 3,
				LinearMipmapNearest = 4,
				LinearMipmapLinear = 5,
				Anisotropic = 6
			};

		public:
			Sampler() noexcept : wrapMode_(Wrap::ClampToEdge) {}
			virtual ~Sampler() noexcept {}

			void setWrapMode(Wrap mode) { wrapMode_ = mode; }
			Wrap getWrapMode() const { return wrapMode_; }

		public:
            virtual T sample(const Texture<T>& texels, float u, float lod) = 0;
            virtual T sample(const Texture<T>& texels, float u, float v, float lod) = 0;
            virtual T sample(const Texture<T>& texels, float u, float v, float w, float lod) = 0;

		private:
			Wrap wrapMode_;
        };

        template<class T>
        class NearestNeighborInterpolation : public Sampler<T>
        {
        public:
            virtual T sample(const Texture<T>& texels, float u, float lod)
            {

            }

            virtual T sample(const Texture<T>& texels, float u, float v, float lod)
            {
				switch (this->getWrapMode())
				{
				case Wrap::Repeat:
				{
					auto x = u - std::floor(u);
					auto y = v - std::floor(v);
					return texels.fetch((int)std::round(x), (int)std::round(y));
				}
				break;
				case Wrap::Mirror:
				{
					auto iu = ((std::uint32_t)std::floor(u));
					auto iv = ((std::uint32_t)std::floor(v));
					auto x = (iu % 2) > 0 ? 1.0f - (u - iu) : (u - iu);
					auto y = (iv % 2) > 0 ? 1.0f - (v - iv) : (v - iv);
					return texels.fetch((int)std::round(x), (int)std::round(y));
				}
				break;
				case Wrap::ClampToEdge:
				{
					auto x = clamp(u, 0.f, 1.f);
					auto y = clamp(v, 0.f, 1.f);
					return texels.fetch((int)std::round(x), (int)std::round(y));
				}
				break;
				}

                return n;
            }

            virtual T sample(const Texture<T>& texels, float u, float v, float w, float lod)
            {
				switch (this->getWrapMode())
				{
				case Wrap::Repeat:
				{
					auto x = u - std::floor(u);
					auto y = v - std::floor(v);
					auto z = z - std::floor(z);
					return texels.fetch((int)std::round(x), (int)std::round(y), (int)std::round(z));
				}
				break;
				case Wrap::Mirror:
				{
					auto iu = ((std::uint32_t)std::floor(u));
					auto iv = ((std::uint32_t)std::floor(v));
					auto iw = ((std::uint32_t)std::floor(w));
					auto x = (iu % 2) > 0 ? 1.0f - (u - iu) : (u - iu);
					auto y = (iv % 2) > 0 ? 1.0f - (v - iv) : (v - iv);
					auto z = (iw % 2) > 0 ? 1.0f - (w - iv) : (w - iv);
					return texels.fetch((int)std::round(x), (int)std::round(y), (int)std::round(z));
				}
				break;
				case Wrap::ClampToEdge:
				{
					auto x = clamp(u, 0.f, 1.f);
					auto y = clamp(v, 0.f, 1.f);
					auto z = clamp(w, 0.f, 1.f);
					return texels.fetch((int)std::round(x), (int)std::round(y), (int)std::round(z));
				}
				break;
				}
            }
        };
    }
}

#endif