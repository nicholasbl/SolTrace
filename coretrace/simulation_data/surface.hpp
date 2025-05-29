#ifndef SOLTRACE_SURFACE_H
#define SOLTRACE_SURFACE_H

#include <memory>

enum SurfaceType
{
    CONE,
    CYLINDER,
    FLAT,
    PARABOLA,
    SPHERE,
};

struct Surface
{
public:
    SurfaceType my_type;

    Surface(SurfaceType st) : my_type(st) {}
    virtual ~Surface() {}

    SurfaceType get_type() { return my_type; }
};

struct Cone : public Surface
{
    double half_angle;
    Cone() : Surface(CONE), half_angle(0.0) {}
    Cone(double ha) : Surface(CONE), half_angle(ha) {}
    virtual ~Cone() {}
};

// TODO: Add needed subfields

struct Cylinder : public Surface
{
    Cylinder() : Surface(CYLINDER) {}
    virtual ~Cylinder() {}
};

struct Flat : public Surface
{
    Flat() : Surface(FLAT) {}
    virtual ~Flat() {}
};

struct Parabola : public Surface
{
    double vertex_x_curv;
    double vertex_y_curv;

    Parabola() : Surface(PARABOLA),
                  vertex_x_curv(0.0),
                  vertex_y_curv(0.0)
    {
    }
    virtual ~Parabola() {}
};

struct Sphere : public Surface
{
    double vertex_curv;

    Sphere() : Surface(SPHERE),
                  vertex_curv(0.0)
    {
    }
    virtual ~Sphere() {}
};

// TODO: Add other surface types.

using surface_ptr = std::shared_ptr<Surface>;

template <typename S, typename... Args>
inline auto make_surface(Args &&...args)
{
    return std::make_shared<S>(std::forward<Args>(args)...);
}

#endif
