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
        class TexImage
        {
        };

        template <typename T>
        class TexImage1D : TexImage<T>
        {
        public:
            TexImage1D(const int &resolution, const T *data, ImageWrap wrapMode = ImageWrap::Repeat);
            std::uint32_t width() const { return resolution; }
            T fetch(const int &u) const;

            const ImageWrap wrapMode;
            int resolution;
            std::vector<T> data;
        };


        template <typename T>
        class TexImage2D : TexImage<T>
        {
        public:
            TexImage2D(const RadeonRays::int2 &resolution, const T *data, ImageWrap wrapMode = ImageWrap::Repeat);
            std::uint32_t width() const { return resolution[0]; }
            std::uint32_t height() const { return resolution[1]; }
            T fetch(const RadeonRays::int2 &uv) const;

            const ImageWrap wrapMode;
            RadeonRays::int2 resolution;
            std::vector<T> data;
        };

        template <typename T>
        class TexImage3D : TexImage<T>
        {
        public:
            TexImage3D(const RadeonRays::int3 &resolution, const T *data, ImageWrap wrapMode = ImageWrap::Repeat);
            std::uint32_t width() const { return resolution[0]; }
            std::uint32_t height() const { return resolution[1]; }
            std::uint32_t length() const { return resolution[2]; }
            T fetch(const RadeonRays::int3 &uvw) const;

            const ImageWrap wrapMode;
            RadeonRays::int3 resolution;
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
		};

        template<class T>
		class Texture1D : public Texture<T>
		{
		public:
			Texture1D() {}
			virtual ~Texture1D() override = default;
			virtual T fetch(float u) const = 0;
		};

        template<class T>
		class Texture2D : public Texture<T>
		{
		public:
			Texture2D() {}
			virtual ~Texture2D() override = default;
			virtual T fetch(float u, float v) const = 0;
		};

        template<class T>
		class Texture3D : public Texture<T>
		{
		public:
			Texture3D() {}
			virtual ~Texture3D() override = default;
			virtual T fetch(float u, float v, float w) const = 0;
		};

        template<class T>
		class TextureCube : public Texture<T>
		{
		public:
			TextureCube() {}
			virtual ~TextureCube() override = default;
			virtual T fetch(float u, float v, float w) const = 0;
		};

        template<class T>
		class ConstantTexture2D : public Texture2D<T>
		{
		public:
			ConstantTexture(const T &value) : value(value) {}
			virtual ~ConstantTexture() override = default;
			virtual T fetch(float u, float v, float w) const override
            {
                return value;
            }

		private:
			T value;
		};

        template<class T>
		class ConstantTexture3D : public Texture3D<T>
		{
		public:
			ConstantTexture(const T &value) : value(value) {}
			virtual ~ConstantTexture() override = default;
			virtual T fetch(float u, float v, float w) const override
            {
                return value;
            }

		private:
			T value;
		};


        template<class T>
		class ImageTexture2D : public Texture2D<T>
		{
		public:
			ImageTexture2D(const TexImage2D & image) : image(tex_image_) {}
			virtual ~ImageTexture2D() override = default;
			virtual T fetch(float u, float v, float w) const override
            {
                return value;
            }

		private:
            const TexImage2D & tex_image_;
		};
	}
}

#endif