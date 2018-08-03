

#include <memmory>
#include <vector>

namespace octoon
{
    class Scene
    {
    public:
        Scene(std::shared_ptr<Primitive> aggregate,
            const std::vector<std::shared_ptr<Light>> &lights)
            : lights(lights), aggregate(aggregate) 
        {
        }
        bool Intersect(const Ray &ray, SurfaceInteraction *isect) const;
        bool IntersectP(const Ray &ray) const;

        std::vector<std::shared_ptr<Light>> lights;
        std::vector<std::shared_ptr<Light>> infiniteLights;
    private:
        std::shared_ptr<Primitive> aggregate;
    };
}