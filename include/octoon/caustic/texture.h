#ifndef OCTOON_CAUSTIC_TEXTURE_H_
#define OCTOON_CAUSTIC_TEXTURE_H_


#include <vector>
#include <cstdint>
#include "math.h"

namespace octoon
{
	namespace caustic
	{
        enum Type
        {
            Texture1D = 0,
            Texture1DArray = 1,
            Texture2D = 2,
            Texture2DArray = 3,
            Texture3D = 4,
            Cube = 5,
            CubeArray = 6
        };
        enum class ImageWrap { Repeat, Black, Clamp };

        template <typename T>
        class Texture
        {
        };

        template <typename T>
        class Texture1D : Texture<T>
        {
        public:
            Texture1D(const int &resolution, const T *data, ImageWrap wrapMode = ImageWrap::Repeat);
            std::uint32_t width() const { return resolution; }
            T fetch(std::uint32_t u) const;

            const ImageWrap wrapMode;
            int resolution;
            std::vector<T> data;
        };


        template <typename T>
        class Texture2D : Texture<T>
        {
        public:
            Texture2D(const RadeonRays::int2 &resolution, const T *data, ImageWrap wrapMode = ImageWrap::Repeat);
            std::uint32_t width() const { return resolution[0]; }
            std::uint32_t height() const { return resolution[1]; }
            T fetch(std::uint32_t u, std::uint32_t v) const;

            const ImageWrap wrapMode;
            RadeonRays::int2 resolution;
            std::vector<T> data;
        };

        template <typename T>
        class Texture3D : Texture<T>
        {
        public:
            Texture3D(const RadeonRays::int3 &resolution, const T *data, ImageWrap wrapMode = ImageWrap::Repeat);
            std::uint32_t width() const { return resolution[0]; }
            std::uint32_t height() const { return resolution[1]; }
            std::uint32_t length() const { return resolution[2]; }
            T fetch(std::uint32_t u, std::uint32_t v, std::uint32_t w) const;

            const ImageWrap wrapMode;
            RadeonRays::int3 resolution;
            std::vector<T> data;
        };
	}
}

#endif