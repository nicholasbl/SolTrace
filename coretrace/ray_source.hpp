#ifndef SOLTRACE_RAY_SOURCE_H
#define SOLTRACE_RAY_SOURCE_H

#include "datetime.hpp"
#include "vector3d.hpp"

class RaySource
{
public:
    RaySource();
    virtual ~RaySource();

    virtual const Vector3d &get_position() const = 0;
    virtual void set_position(const Vector3d &) = 0;
    virtual void set_position(const DateTime &, double lat, double long) = 0;
    virtual void get_shape() = 0;
    virtual void set_shape() = 0;

private:
};

#endif
