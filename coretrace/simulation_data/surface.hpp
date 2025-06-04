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
    // z(x,y) = sqrt(x^2 + y^2) / tan(theta)
    // where theta = half_angle
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
    // z(x,y) = (cx * x^2 + cy * y^2) / 2
    // TODO: Assuming that vertex_x_curv gives cx and
    // that vertex_y_curv gives cy
    double vertex_x_curv;
    double vertex_y_curv;

    Parabola() : Surface(PARABOLA),
                 vertex_x_curv(0.0),
                 vertex_y_curv(0.0)
    {
    }
    Parabola(double x_curv, double y_curv) : Surface(PARABOLA),
                                             vertex_x_curv(x_curv),
                                             vertex_y_curv(y_curv)
    {
    }
    virtual ~Parabola() {}
};

struct Sphere : public Surface
{
    // z(x,y) = c(x^2 + y^2) / [1 + sqrt(1 - c^2{x^2 + y^2})]
    // where c = 1/R.
    // TODO: Assume vertex_curv gives 1/R.
    double vertex_curv;

    Sphere() : Surface(SPHERE),
               vertex_curv(0.0)
    {
    }
    Sphere(double curv) : Surface(SPHERE),
                          vertex_curv(curv)
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
