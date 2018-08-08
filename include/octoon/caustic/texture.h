#ifndef OCTOON_CAUSTIC_TEXTURE_H_
#define OCTOON_CAUSTIC_TEXTURE_H_


#include <vector>
#include <cstdint>
#include "math.h"

namespace octoon
{
	namespace caustic
	{
        enum class ImageWrap { Repeat, Black, Clamp };
        template <typename T>
        class TextureImage
        {
        public:
            TextureImage(const RadeonRays::int2 &resolution, const T *data, ImageWrap wrapMode = ImageWrap::Repeat);
            std::uint32_t width() const { return resolution[0]; }
            std::uint32_t height() const { return resolution[1]; }
            T lookup(const RadeonRays::int2 &uv) const;

            const ImageWrap wrapMode;
            RadeonRays::int2 resolution;
            std::vector<T> data;
        };


		template<class T>
		class Texture
		{
		public:
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

		public:
			virtual ~Texture() = default;
			virtual T fetch(std::uint32_t u, std::uint32_t v, std::uint32_t w) const = 0;
		};

		template<class T>
		class ConstantTexture : public Texture<T>
		{
		public:
			ConstantTexture(const T &value) : value(value) {}
			virtual ~ConstantTexture() override = default;
			virtual T fetch(std::uint32_t u, std::uint32_t v, std::uint32_t w) const override
            {
                return value;
            }

		private:
			T value;
		};

        template<class T>
		class Texture2D : public Texture<T>
		{
		public:
			Texture2D() {}
			virtual ~Texture2D() override = default;
			virtual T fetch(std::uint32_t u, std::uint32_t v, std::uint32_t w) const override
            {
                return value;
            }

		private:
			std::vector<T> value;
		};

        template<class T>
		class Texture3D : public Texture<T>
		{
		public:
			Texture3D() {}
			virtual ~Texture3D() override = default;
			virtual T fetch(std::uint32_t u, std::uint32_t v, std::uint32_t w) const override
            {
                return value;
            }

		private:
			std::vector<T> value;
		};

        template<class T>
		class TextureCube : public Texture<T>
		{
		public:
			TextureCube() {}
			virtual ~TextureCube() override = default;
			virtual T fetch(std::uint32_t u, std::uint32_t v, std::uint32_t w) const override
            {
                return value;
            }

		private:
			std::vector<T> value;
		};
	}
}

#endif