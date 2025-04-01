#ifndef SOLTRACE_SUN_H
#define SOLTRACE_SUN_H

#include "ray_source.hpp"
#include "vector3d.hpp"

class Sun: public RaySource
{
public:

    Sun();
    virtual ~Sun();

    virtual const Vector3d &get_position() const;
    virtual void set_position(const Vector3d &);
    virtual void set_position(const DateTime &, double lat, double long);
    virtual void get_shape();
    virtual void set_shape();

private:
};

#endif
