
#include "plane_reflector.hpp"

#include <memory>

#include "shape.hpp"
#include "vector3d.hpp"

PlaneReflector::PlaneReflector() : length(1.0), width(1.0)
{
    this->shape = std::make_unique<Plane>();
    return;
}

int PlaneReflector::set_bounding_box()
{
    int sts = 0;
    return sts;
}

int PlaneReflector::update_orientation(const DateTime &dt,
                                       const Vector3d &source,
                                       const Vector3d &target)
{
    int sts = 0;

    // aim = 0.5 * (target - source)
    vector_add(0.5, target, -0.5, source, this->aim);

    return sts;
}
