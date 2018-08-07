#ifndef OCTOON_SAMPLER
#define OCTOON_SAMPLER

#include "math.h"
#include "texture.h"

namespace octoon
{
    class Sampler
    {
    public:
        virtual void sample(const Texture &origin, float u, float lod) = 0;
        virtual void sample(const Texture &origin, float u, float v, float lod) = 0;
        virtual void sample(const Texture &origin, float u, float v, float w, float lod) = 0;
    };
} // namespace octoon
#endif // OCTOON_SAMPLER