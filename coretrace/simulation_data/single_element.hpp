#ifndef SOLTRACE_PLANE_H
#define SOLTRACE_PLANE_H

#include "element.hpp"

#include <memory>

#include "aperature.hpp"
#include "surface.hpp"

class SingleElement : public ElementBase
{
public:
    SingleElement();
    virtual ~SingleElement();

    virtual bool is_single() const { return true; }

    virtual const aperature_ptr get_aperature() const
    {
        return this->aperature;
    }
    virtual aperature_ptr get_aperature()
    {
        return this->aperature;
    }
    virtual void set_aperature(aperature_ptr ap)
    {
        this->aperature = ap;
        return;
    }

    virtual const surface_ptr get_surface() const
    {
        return this->surface;
    }
    virtual surface_ptr get_surface()
    {
        return this->surface;
    }
    virtual void set_surface(surface_ptr sp)
    {
        this->surface = sp;
    }

    const OpticalProperties *get_front_optical_properties() const
    {
        return &(this->optics_front);
    }
    OpticalProperties *get_front_optical_properties()
    {
        return &(this->optics_front);
    }
    void set_front_optical_properties(const OpticalProperties &op)
    {
        this->optics_front = op;
    }

    const OpticalProperties *get_back_optical_properties() const
    {
        return &(this->optics_back);
    }
    OpticalProperties *get_back_optical_properties()
    {
        return &(this->optics_back);
    }
    void set_back_optical_properties(const OpticalProperties &op)
    {
        this->optics_back = op;
    }

    virtual int update_orientation(const DateTime &dt,
                                   const Vector3d &source,
                                   const Vector3d &target);

protected:
    aperature_ptr aperature;
    surface_ptr surface;

    OpticalProperties optics_front;
    OpticalProperties optics_back;
};

#endif
