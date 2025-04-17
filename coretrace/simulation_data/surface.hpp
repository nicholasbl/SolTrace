#ifndef SOLTRACE_SURFACE_H
#define SOLTRACE_SURFACE_H

#include <memory>

enum SurfaceType
{
    CONICAL,
    CYLINDER,
    FLAT,
    PARABOLIC,
    SPHERICAL,
};

struct Surface
{
public:
    SurfaceType my_type;

    Surface(SurfaceType st) : my_type(st) {}
    virtual ~Surface() {}

    SurfaceType get_type() { return my_type; }
};

struct Conical : public Surface
{
    double half_angle;
    Conical() : Surface(CONICAL), half_angle(0.0) {}
    Conical(double ha) : Surface(CONICAL), half_angle(ha) {}
    virtual ~Conical() {}
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

struct Parabolic : public Surface
{
    Parabolic() : Surface(PARABOLIC) {}
    virtual ~Parabolic() {}
};

struct Spherical : public Surface
{
    Spherical() : Surface(SPHERICAL) {}
    virtual ~Spherical() {}
};

// TODO: Add other surface types.

using surface_ptr = std::shared_ptr<Surface>;

template <typename S, typename... Args>
inline auto make_surface(Args &&...args)
{
    return std::make_shared<S>(std::forward<Args>(args)...);
}

#endif
