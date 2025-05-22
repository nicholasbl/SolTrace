
#include "single_element.hpp"

#include <memory>

#include "aperture.hpp"
#include "element.hpp"
#include "vector3d.hpp"

SingleElement::SingleElement() : ElementBase(),
                                 aperture(nullptr),
                                 surface(nullptr),
                                 optics_front(),
                                 optics_back()
{
    // TODO: Do we want a default aperture and surface? What about default
    // optical properties?
    return;
}

SingleElement::~SingleElement()
{
    this->aperture = nullptr;
    this->surface = nullptr;
    return;
}

int SingleElement::update_orientation(const DateTime &dt,
                                      const Vector3d &source,
                                      const Vector3d &target)
{
    int sts = 0;
    // TODO: Implment this
    return sts;
}
