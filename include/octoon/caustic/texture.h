#ifndef OCTOON_CAUSTIC_TEXTURE_H_
#define OCTOON_CAUSTIC_TEXTURE_H_

#include "math.h"

namespace octoon
{
	namespace caustic
	{
		template<class T>
		class Texture
		{
		public:
			enum Type
			{
				Texture2D = 0,
				Texture2DArray = 1,
				Texture3D = 2,
				Cube = 3,
				CubeArray = 4,
				Latlong = 5, // environment hdri
				FishEye
			};

		public:
			virtual ~Texture() = default;
			virtual T fetch(float u, float v, float w) const = 0;
		};

		template<class T>
		class ConstantTexture : public Texture<T>
		{
		public:
			ConstantTexture(const T &value) : value(value) {}
			virtual ~ConstantTexture() = default;
			virtual T fetch(float u, float v, float w) const override { return value; }

		private:
			T value;
		};
	}
}

#endif