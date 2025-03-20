#ifndef SOLTRACE_PLANE_H
#define SOLTRACE_PLANE_H

#include "element.hpp"

class PlaneReflector : public ElementBase
{
public:
    PlaneReflector();
    PlaneReflector(double length, double width, const Vector3d &normal);
    virtual ~PlaneReflector();

    virtual int set_bounding_box();
    virtual int update_orientation(const DateTime &dt,
                                   const Vector3d &source,
                                   const Vector3d &target);

private:
    double length;
    double width;
};

#endif
