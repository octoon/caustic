#ifndef OCTOON_CAUSTIC_TEXTURE_H_
#define OCTOON_CAUSTIC_TEXTURE_H_

#include <vector>
#include <cstdint>
#include "math.h"

namespace octoon
{
	namespace caustic
	{
		template<typename T>
		class Texture
		{
		public:
			enum Type
			{
                Image1D = 0,
                Image1DArray = 1,
				Image2D = 2,
				Image2DArray = 3,
				Image3D = 4,
				Cube = 5,
				CubeArray = 6
			};

		public:
			Texture() = default;
			virtual ~Texture() = default;

			// resolution
            virtual std::uint32_t width() const noexcept { return 1; }
            virtual std::uint32_t height() const noexcept { return 1; }
            virtual std::uint32_t depth() const noexcept { return 1; }

            // LOD
			virtual std::uint8_t minLevel() const noexcept { return 1; }
			virtual std::uint8_t maxLevel() const noexcept { return 1; }

			// Layer
			virtual std::uint8_t minLayer() const noexcept { return 0; }
			virtual std::uint8_t maxLayer() const noexcept { return 0; }
		};

        template <typename T>
        class Texture1D : Texture<T>
        {
        public:
            Texture1D(const std::uint32_t &resolution, const T *data);

            std::uint32_t width() const override { return resolution; }

            T fetch(std::uint32_t x, std::uint32_t lod) const noexcept override
            {
            	return data[x];
            };

            T fetch(std::uint32_t x, std::uint32_t y, std::uint32_t lod) const noexcept override
            {
            	assert(y == 0);
            	return data[x];
            };

            T fetch(std::uint32_t x, std::uint32_t y, std::uint32_t z, std::uint32_t lod) const noexcept override
            {
            	assert(y == 0 && z == 0);
            	return data[x];
            };

        private:
            std::uint32_t resolution;
            std::vector<T> data;
        };

        template <typename T>
        class Texture2D final : Texture<T>
        {
        public:
			Texture2D(std::uint32_t w, std::uint32_t h, const T *data)
			{
			}
			virtual ~Texture2D() = default;

            std::uint32_t width() const override { return width_; }
            std::uint32_t height() const override { return height_; }

            T fetch(std::uint32_t x, std::uint32_t lod) const noexcept override
            {
            	return data[x];
            };

            T fetch(std::uint32_t x, std::uint32_t y, std::uint32_t lod) const noexcept override
            {
            	return data[x * width_ + y];
            };

            T fetch(std::uint32_t x, std::uint32_t y, std::uint32_t z, std::uint32_t lod) const noexcept override
            {
            	assert(z == 0);
            	return data[x * width_ + y];
            };

        private:
            std::uint32_t width_;
            std::uint32_t height_;
            std::vector<T> data;
        };

		template <typename T>
		class Texture2DArray final : Texture<T>
		{
		public:
			Texture2DArray(std::uint32_t w, std::uint32_t h, std::uint32_t layer, const T *data);

			std::uint32_t width() const override { return width_; }
			std::uint32_t height() const override { return height_; }
			std::uint32_t maxLayer() const override { return layer_; }

			T fetch(std::uint32_t x, std::uint32_t y, std::uint32_t z, std::uint32_t lod) const noexcept override
			{
				return data_[z][x * width_ + y];
			};

		private:
			std::uint32_t width_;
			std::uint32_t height_;
			std::uint32_t layer_;
			std::vector<std::unique_ptr<Texture2D<T>>> data_;
		};

        template <typename T>
        class Texture3D final : Texture<T>
        {
        public:
            Texture3D(std::uint32_t w, std::uint32_t h, std::uint32_t depth, const T *data);

            std::uint32_t width() const override { return width_; }
            std::uint32_t height() const override { return height_; }
            std::uint32_t length() const override { return depth_; }

            T fetch(std::uint32_t x, std::uint32_t y, std::uint32_t z, std::uint32_t lod) const noexcept override
            {
            	return data_[z * depth_ * width_ + x * width_ + y];
            };

        private:
            std::uint32_t width_;
            std::uint32_t height_;
            std::uint32_t depth_;
            std::vector<T> data_;
        };

        template<class T>
		class TextureCube final : public Texture<T>
		{
		public:
			TextureCube() {}
			virtual ~TextureCube() override = default;

			virtual T fetch(std::uint32_t x, std::uint32_t y, std::uint32_t z, std::uint32_t lod) const noexcept override
			{
			}
		};

        template<class T>
		class TextureCubeArray final : public Texture<T>
		{
		public:
			TextureCubeArray() {}
			virtual ~TextureCubeArray() override = default;

			virtual T fetch(std::uint32_t x, std::uint32_t y, std::uint32_t z, std::uint32_t lod) const noexcept override
			{
			}
		};
	}
}

#endif