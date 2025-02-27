#ifndef SOLTRACE_RAY_SOURCE_H
#define SOLTRACE_RAY_SOURCE_H

#include "datetime.hpp"

class RaySource
{
public:
    RaySource();
    virtual ~RaySource();

    virtual void get_position() const = 0;
    virtual void set_position(const double*, unsigned long len) = 0;
    virtual void set_position(DateTime&, double latitude, double longitude) = 0;
    virtual void get_shape() const = 0;
    virtual void set_shape() = 0;

private:
};

#endif