
#include "single_element.hpp"

#include <memory>

#include "aperature.hpp"
#include "element.hpp"
#include "vector3d.hpp"

SingleElement::SingleElement()
    : ElementBase(), optics_front(), optics_back()
{
    this->aperature = nullptr;
    this->surface = nullptr;
    return;
}

// SingleElement::SingleElement(double height,
//                              double width,
//                              const Vector3d &origin,
//                              const Vector3d &normal)
//     : ElementBase(origin, normal)
// {
//     this->aperature = std::make_unique<Rectangle>(height, width);
//     this->surface = std::make_unique<Flat>();
//     return;
// }

SingleElement::~SingleElement()
{
    this->aperature = nullptr;
    this->surface = nullptr;
    return;
}

// int SingleElement::set_bounding_box()
// {
//     int sts = 0;
//     return sts;
// }

int SingleElement::update_orientation(const DateTime &dt,
                                      const Vector3d &source,
                                      const Vector3d &target)
{
    int sts = 0;

    return sts;
}
