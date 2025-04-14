
#include "composite_element.hpp"

CompositeElement::CompositeElement() : ElementBase(), my_elements()
{
    return;
}

CompositeElement::~CompositeElement()
{
    // NOTE: ElementContainer should handle all the necessary tear down here
    return;
}

int CompositeElement::update_orientation(const DateTime &dt,
                                         const Vector3d &source,
                                         const Vector3d &target)
{
    // TODO: Implement this
    int sts = 0;
    return sts;
}