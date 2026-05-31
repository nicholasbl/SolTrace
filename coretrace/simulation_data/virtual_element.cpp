
#include "virtual_element.hpp"

namespace SolTrace::Data {


VirtualElement::VirtualElement() 
    : SingleElement()
{
    this->opt_id = OPTICS_ID_VIRTUAL;
}

VirtualElement::~VirtualElement()
{
}

VirtualPlane::VirtualPlane(double x_len, double y_len) : VirtualElement()
{
    this->aperture = make_aperture<Rectangle>(x_len, y_len);
    this->surface = make_surface<Flat>();
}

VirtualPlane::~VirtualPlane()
{
}

} // namespace SolTrace::Data
