#include "stage_element.hpp"

#include "element.hpp"

namespace SolTrace::Data {

StageElement::StageElement(int_fast64_t stage) : CompositeElement()
{
    this->set_stage(stage);
    return;
}

StageElement::~StageElement()
{
    return;
}


element_id StageElement::add_element(element_ptr el)
{
    element_id id = this->CompositeElement::add_element(el);
    if (Element::is_success(id))
    {
        el->set_stage(this->stage);
    }
    return id;
}

} // namespace SolTrace::Data
