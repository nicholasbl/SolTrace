#ifndef SOLTRACE_PLANE_H
#define SOLTRACE_PLANE_H

#include "element.hpp"

class PlaneReflector : public ElementBase
{
public:
    PlaneReflector();
    PlaneReflector(double height,
                   double width,
                   const Vector3d &origin,
                   const Vector3d &normal);
    virtual ~PlaneReflector();

    // virtual int set_bounding_box();
    virtual int update_orientation(const DateTime &dt,
                                   const Vector3d &source,
                                   const Vector3d &target);

private:
};

#endif
