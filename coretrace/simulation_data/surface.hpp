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
    Cone(double ha) : Surface(CONE), half_angle(ha) {}
    virtual ~Cone() {}
};

struct Cylinder : public Surface
{
    // x^2 + (z - r)^2 = r^2
    // where r = radius
    double radius;
    Cylinder(double r) : Surface(CYLINDER), radius(r)
    {
    }
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
    double focal_length_x;
    double focal_length_y;

    Parabola(double focal_x, double focal_y) : Surface(PARABOLA),
                                               focal_length_x(focal_x),
                                               focal_length_y(focal_y)
    {
    }
    virtual ~Parabola() {}
};

// TODO: Add needed subfields

struct Sphere : public Surface
{
    // z(x,y) = c(x^2 + y^2) / [1 + sqrt(1 - c^2{x^2 + y^2})]
    // where c = 1/R.
    // TODO: This form seems to be unnecessarily complicated
    // and also gives only the bottom half of the sphere.
    // Could easily just use one of equation
    // z(x,y) = (1 - sqrt(1 - c^2 (x^2 + y^2))) / c
    //        = R * (1 - sqrt(R^2 - (x^2 + y^2)))
    // Need to check on this.
    // TODO: Do we need the top half of the sphere?
    double vertex_curv;

    Sphere(double curv) : Surface(SPHERE),
                          vertex_curv(curv)
    {
    }
    virtual ~Sphere() {}
};

// TODO: Add other surface types. Documentation has the following:
// 1. Hyperboloid/Ellipsoid
// 2. Zernike Series
// 3. VSHOT data
// 4. Finite Element data
// 5. General Spencer & Murty Equation
// 6. Polynomial Series (rotationally symmetric)
// 7. Cubic Spline Interpolation (rotationally symmetric)

using surface_ptr = std::shared_ptr<Surface>;

template <typename S, typename... Args>
inline auto make_surface(Args &&...args)
{
    return std::make_shared<S>(std::forward<Args>(args)...);
}

#endif
