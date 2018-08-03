

#include <octoon_ray/math/math.h>
#include "pixel.h"


namespace octoon
{
    class Film
    {
    public:
        Film(const math::int2 &resolution);

        Pixel &getPixel(int x, int y) {
            return pixels[y*resolution.x + x];
        }

    private:
        std::unique_ptr<Pixel[]> pixels;
        const math::int2 resolution;
    };
}