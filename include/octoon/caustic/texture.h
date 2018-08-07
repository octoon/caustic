#ifndef OCTOON_CAUSTIC_TEXTURE_H_
#define OCTOON_CAUSTIC_TEXTURE_H_

#include "math.h"

namespace octoon
{
    template<class T>
    class Texture
    {
    public:
        virtual T evaluate(float u, float v, float w) const = 0;
        virtual ~Texture() = default;
    };

    template<class T>
    class ConstantTexture : public Texture<T>
    {
    public:
        ConstantTexture(const T &value) : value(value) {}
        virtual ~ConstantTexture() = default;
        virtual T evaluate(float u, float v, float w) const override { return value; }

    private:
        T value;
    };
} // namespace octoon
#endif