

#include <memory>

#include <octoon_ray/math/math.h>
#include "film.h"

namespace octoon
{
    class Camera
    {
    public:
        Camera(const math::Matrix4x4 &CameraToWorld, std::shared_ptr<Film> film);
        virtual ~Camera();
        virtual float generateRay(const math::float2 &sample, Ray *ray) const = 0;
        math::Matrix4x4 CameraToWorld;
        std::shared_ptr<Film> film;
    };

}