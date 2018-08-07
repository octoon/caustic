#ifndef OCTOON_CAUSTIC_TEXTURE_H_
#define OCTOON_CAUSTIC_TEXTURE_H_

#include "math.h"

namespace octoon
{
    class Texture
    {
    public:
        virtual float evaluate(float u, float v, float w) const = 0;
        virtual ~Texture() = default;
    };

    class ConstantTexture : public Texture
    {
    public:
        ConstantTexture(const float &value) : value(value) {}
        virtual ~ConstantTexture() = default;
        virtual float evaluate(float u, float v, float w) const override { return value; }

    private:
        float value;
    };
} // namespace octoon
#endif