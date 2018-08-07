#ifndef OCTOON_CAUSTIC_SAMPLER
#define OCTOON_CAUSTIC_SAMPLER

#include <algorithm>

#include "math.h"
#include "texture.h"

namespace octoon
{
    namespace caustic
    {
        template<class T>
        class Sampler
        {
        public:
            virtual T sample(const Texture<T> &origin, float u, float lod) = 0;
            virtual T sample(const Texture<T> &origin, float u, float v, float lod) = 0;
            virtual T sample(const Texture<T> &origin, float u, float v, float w, float lod) = 0;
        };

        template<class T>
        class NearestNeighborInterpolation : public Sampler<T>
        {
        public:
            virtual T sample(const Texture<T> &origin, float u, float lod)
            {

            }
            virtual T sample(const Texture<T> &origin, float u, float v, float lod)
            {
                auto x = clamp(u, 0.f, 1.f);
                auto y = clamp(v, 0.f, 1.f);
                auto n = origin.evaluate((int)std::round(x), (int)std::round(y));

                return n;
            }
            virtual T sample(const Texture<T> &origin, float u, float v, float w, float lod)
            {
                auto x = clamp(u, 0.f, 1.f);
                auto y = clamp(v, 0.f, 1.f);
                auto z = clamp(w, 0.f, 1.f);
                auto n = origin.evaluate((int)std::round(x), (int)std::round(y), (int)std::round(z));

                return n;
            }
        };
    }
} // namespace octoon
#endif // OCTOON_CAUSTIC_SAMPLER