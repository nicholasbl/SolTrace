
#include "plane_reflector.hpp"

#include <memory>

#include "aperature.hpp"
#include "element.hpp"
#include "vector3d.hpp"

PlaneReflector::PlaneReflector()
{
    this->aperature = std::make_unique<Rectangle>(1.0, 1.0);
    this->aim.zero();
    return;
}

PlaneReflector::PlaneReflector(double height,
                               double width,
                               const Vector3d &origin,
                               const Vector3d &normal)
    : ElementBase(origin, normal)
{
    this->aperature = std::make_unique<Rectangle>(height, width);
    this->surface = std::make_unique<Flat>();
    return;
}

PlaneReflector::~PlaneReflector()
{
    return;
}

// int PlaneReflector::set_bounding_box()
// {
//     int sts = 0;
//     return sts;
// }

int PlaneReflector::update_orientation(const DateTime &dt,
                                       const Vector3d &source,
                                       const Vector3d &target)
{
    int sts = 0;

    // aim = 0.5 * (target - source)
    vector_add(0.5, target, -0.5, source, this->aim);

    return sts;
}
