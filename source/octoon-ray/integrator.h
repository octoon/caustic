#include "scene.h"



namespace octoon
{
    class Integrator 
    {
    public:
        // Integrator Interface
        virtual void render(const Scene &scene) = 0;
    };


    class SamplerIntegrator : public Integrator
    {
    public:
        SamplerIntegrator(std::shared_ptr<Camera> camera,
                        std::shared_ptr<Sampler> sampler,
                        const int2 &pixelBounds)
            : camera(camera), sampler(sampler), pixelBounds(pixelBounds) {}
        void Render(const Scene &scene);
    protected:
        std::shared_ptr<const Camera> camera;
    private:
        std::shared_ptr<Sampler> sampler;
        const int2 pixelBounds;
    };
}