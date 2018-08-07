#ifndef CAUSTIC_SAMPLER
#define CAUSTIC_SAMPLER

#include <algorithm>

#include "math.h"
#include "texture.h"

namespace octoon
{
    namespace caustic
    {
        class Sampler
        {
        public:
            virtual void sample(const Texture &origin, float u, float lod) = 0;
            virtual void sample(const Texture &origin, float u, float v, float lod) = 0;
            virtual void sample(const Texture &origin, float u, float v, float w, float lod) = 0;
        };

        class NearestNeighborInterpolation : public Sampler
        {
        public:
            virtual void sample(const Texture &origin, float u, float lod)
            {

            }
            virtual void sample(const Texture &origin, float u, float v, float lod)
            {
                auto x = clamp(u, 0.f, 1.f) * this->width;
                auto y = clamp(v, 0.f, 1.f) * this->height;
                auto n = (this->width * (int)std::round(y) + (int)std::round(x)) * this->channel;

                return Vector3<T>(this->data[n], this->data[n + 1], this->data[n + 2]);
            }
            virtual void sample(const Texture &origin, float u, float v, float w, float lod)
            {

            }
        };
    }
} // namespace octoon
#endif // CAUSTIC_SAMPLER