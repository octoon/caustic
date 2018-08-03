
namespace octoon
{
    class Primitive 
    {
    public:
        virtual ~Primitive();
        virtual bool Intersect(const Ray &r, SurfaceInteraction *) const = 0;
        virtual bool IntersectP(const Ray &r) const = 0;
        virtual const Material *GetMaterial() const = 0;
    };
}

