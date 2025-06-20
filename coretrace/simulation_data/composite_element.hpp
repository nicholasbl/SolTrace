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

    virtual void disable() const;
    virtual void enable() const;

    virtual bool is_composite() const
    {
        return true;
    }

    virtual void set_stage(int_fast64_t stage);

    virtual uint_fast64_t get_number_of_elements() const
    {
        // return this->my_elements.get_number_of_items();
        return this->number_of_elements;
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
    element_id add_element(element_ptr el);
    uint_fast64_t remove_element(element_id id);
    element_ptr get_element(element_id id);
    bool replace_element(element_id id, element_ptr el);
    void clear();

    // uint64_t get_total_number_of_elements() const
    // {
    //     return this->my_elements.get_total_number_of_items();
    // }

    virtual ElementContainer::iterator get_iterator()
    {
        return this->my_elements.get_iterator();
    }
    virtual ElementContainer::const_iterator get_const_iterator() const
    {
        return this->my_elements.get_const_iterator();
    }
    virtual bool is_at_end(ElementContainer::iterator iter)
    {
        return this->my_elements.is_at_end(iter);
    }
    virtual bool is_at_end(ElementContainer::const_iterator citer) const
    {
        return this->my_elements.is_at_end(citer);
    }

private:
    uint_fast64_t number_of_elements;
    ElementContainer my_elements;
};

using composite_element_ptr = std::shared_ptr<CompositeElement>;

class StageElement: public CompositeElement
{
public:
    StageElement(int_fast64_t stage);
    ~StageElement();
    virtual bool is_stage() const { return true; }
    virtual element_id add_element(element_ptr el);
private:
};

using stage_ptr = std::shared_ptr<StageElement>;
template <typename... Args>
inline auto make_stage(Args &&...args)
{
    return make_element<StageElement>(std::forward<Args>(args)...);
}

#endif
