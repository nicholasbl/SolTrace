#ifndef SOLTRACE_COMPOSITE_ELEMENT_H
#define SOLTRACE_COMPOSITE_ELEMENT_H

#include "container.hpp"
#include "element.hpp"

class CompositeElement : public ElementBase
{
public:
    
    CompositeElement();
    virtual ~CompositeElement();

    element_id add_item(element_ptr el)
    {
        return this->my_elements.add_item(el);
    }
    auto remove_item(element_id id)
    {
        return this->my_elements.remove_item(id);
    }
    element_ptr get_item(element_id id)
    {
        return this->my_elements.get_item(id);
    }
    auto replace_item(element_id id, element_ptr el)
    {
        return this->my_elements.replace_item(id, el);
    }

    uint64_t get_number_of_elements() const
    {
        return this->my_elements.get_number_of_items();
    }
    uint64_t get_total_number_of_elements() const
    {
        return this->my_elements.get_total_number_of_items();
    }

    ElementContainer::iterator get_iterator()
    {
        return this->my_elements.get_iterator();
    }
    ElementContainer::const_iterator get_const_iterator()
    {
        return this->my_elements.get_const_iterator();
    }
    bool is_at_end(ElementContainer::iterator iter)
    {
        return this->my_elements.is_at_end(iter);
    }
    bool is_at_end(ElementContainer::const_iterator citer)
    {
        return this->my_elements.is_at_end(citer);
    }

private:

    ElementContainer my_elements;

};

#endif
