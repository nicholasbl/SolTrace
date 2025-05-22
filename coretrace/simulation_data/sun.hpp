#ifndef SOLTRACE_SUN_H
#define SOLTRACE_SUN_H

#include "ray_source.hpp"
#include "vector3d.hpp"

class Sun : public RaySource
{
public:
    Sun() { this->my_position.zero(); }
    virtual ~Sun() {}

    virtual const Vector3d &get_position() const
    {
        return this->my_position;
    }
    virtual Vector3d &get_position()
    {
        return this->my_position;
    }
    virtual void set_position(const Vector3d &pos)
    {
        this->my_position = pos;
        return;
    }
    virtual void set_position(double x, double y, double z)
    {
        this->my_position.set_values(x, y, z);
        return;
    }
    virtual void set_position(const DateTime &, double lat, double long) {}
    virtual void get_shape() {}
    virtual void set_shape() {}

private:
    Vector3d my_position;
};

#endif
