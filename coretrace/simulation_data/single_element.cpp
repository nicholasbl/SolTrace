
#include "single_element.hpp"

#include <exception>
#include <memory>
#include <sstream>

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

void SingleElement::enforce_user_fields_set() const
{
    ElementBase::enforce_user_fields_set();

    if (this->aperture == nullptr)
    {
        std::stringstream ss;
        ss << "Element (Name: " << this->get_name()
           << ", UUID: " << this->get_id()
           << ") has no aperture.";
        throw std::invalid_argument(ss.str());
    }

    if (this->surface == nullptr)
    {
        std::stringstream ss;
        ss << "Element (Name: " << this->get_name()
           << ", UUID: " << this->get_id()
           << ") has no surface.";
        throw std::invalid_argument(ss.str());
    }

    // TODO: Add optics checks here

    return;
}
