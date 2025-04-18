#ifndef SOLTRACE_VIRTUAL_ELEMENT_H
#define SOLTRACE_VIRTUAL_ELEMENT_H

#include "single_element.hpp"

class VirtualElement : public SingleElement
{
public:
    static void set_virtual_optics(OpticalProperties &op);

    VirtualElement();
    virtual ~VirtualElement();

    virtual bool is_virtual() { return true; }

private:
    void set_front_optical_properties(const OpticalProperties &op) {}
    void set_back_optical_properties(const OpticalProperties &op) {}
};

class VirtualPlane : public VirtualElement
{
public:
    VirtualPlane(double x_len, double y_len);
    virtual ~VirtualPlane();

private:
    void set_aperature(aperature_ptr ap) {}
    void set_surface(surface_ptr sp) {}
};

#endif