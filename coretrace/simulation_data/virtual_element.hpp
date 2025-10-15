/**
 * @file virtual_element.hpp
 * @brief Virtual element for non-interacting ray tracking
 *
 * Defines virtual elements that can track rays without optical
 * interaction, useful for coordinate system definitions and ray tracking.
 * Virtual elements allow rays to pass through without reflection,
 * refraction, or absorption, while still recording ray intersections.
 */

#ifndef SOLTRACE_VIRTUAL_ELEMENT_H
#define SOLTRACE_VIRTUAL_ELEMENT_H

#include "single_element.hpp"

namespace SolTrace::Data {

class VirtualElement : public SingleElement
{
public:
    static void set_virtual_optics(OpticalProperties &op);

    VirtualElement();
    virtual ~VirtualElement();

    virtual bool is_virtual() const override { return true; }

    void set_front_optical_properties(const OpticalProperties &op) override {}
    void set_back_optical_properties(const OpticalProperties &op) override {}

private:
};

class VirtualPlane : public VirtualElement
{
public:
    VirtualPlane(double x_len, double y_len);
    virtual ~VirtualPlane();

    void set_aperture(aperture_ptr ap) {}
    void set_surface(surface_ptr sp) {}

private:
};

} // namespace SolTrace::Data

#endif
