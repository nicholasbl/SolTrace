#ifndef SOLTRACE_COMPOSITE_ELEMENT_H
#define SOLTRACE_COMPOSITE_ELEMENT_H

#include <memory>

#include "container.hpp"
#include "element.hpp"

class CompositeElement : public ElementBase
{
public:
    CompositeElement();
    virtual ~CompositeElement();

    virtual bool is_composite() const
    {
        return true;
    }

    // Element interface functions
    virtual const aperture_ptr get_aperture() const { return nullptr; }
    virtual aperture_ptr get_aperture() { return nullptr; }
    virtual void set_aperture(aperture_ptr) {}

    virtual const surface_ptr get_surface() const { return nullptr; }
    virtual surface_ptr get_surface() { return nullptr; }
    virtual void set_surface(surface_ptr) {}

    virtual const OpticalProperties *get_front_optical_properties() const
    {
        return nullptr;
    }
    virtual OpticalProperties *get_front_optical_properties()
    {
        return nullptr;
    }
    virtual void set_front_optical_properties(const OpticalProperties &) {}

    virtual const OpticalProperties *get_back_optical_properties() const
    {
        return nullptr;
    }
    virtual OpticalProperties *get_back_optical_properties()
    {
        return nullptr;
    }
    virtual void set_back_optical_properties(const OpticalProperties &) {};

    virtual int update_orientation(const DateTime &,
                                   const Vector3d &source,
                                   const Vector3d &target);

    // CompositeElement accessors
    element_id add_element(element_ptr el)
    {
        return this->my_elements.add_item(el);
    }
    auto remove_element(element_id id)
    {
        return this->my_elements.remove_item(id);
    }
    element_ptr get_element(element_id id)
    {
        return this->my_elements.get_item(id);
    }
    auto replace_element(element_id id, element_ptr el)
    {
        return this->my_elements.replace_item(id, el);
    }

    uint64_t get_number_of_elements() const
    {
        return this->my_elements.get_number_of_items();
    }
    // uint64_t get_total_number_of_elements() const
    // {
    //     return this->my_elements.get_total_number_of_items();
    // }

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

using composite_element_ptr = std::shared_ptr<CompositeElement>;

#endif
