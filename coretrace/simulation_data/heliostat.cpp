
#include "heliostat.hpp"

#include <exception>

Heliostat::Heliostat()
    : CompositeElement(),
      initialized(false)
{
    // TODO: Initialize to nonsense and enforce user setting of values
    // TODO: Need to do something similar for elements totally
    return;
}

Heliostat::~Heliostat()
{
    return;
}

void Heliostat::create_geometry()
{
    if (this->initialized)
        return;

    double panel_len_x = this->aperture_size_x -
                         this->gap_x * (this->num_panels_x - 1);
    panel_len_x /= this->num_panels_x;
    double panel_len_y = this->aperture_size_y -
                         this->gap_y * (this->num_panels_y - 1);
    panel_len_y /= this->num_panels_y;

    if (this->canting_method == OFF_AXIS)
    {
        // TODO: Do stuff here...
    }

    this->heliostat_area = 0.0;
    double panel_x = -0.5 * (this->aperture_size_x - panel_len_x);
    double panel_y = -0.5 * (this->aperture_size_y - panel_len_y);

    single_element_ptr elem;
    Vector3d origin;
    Vector3d aim;
    aperture_ptr ap;
    surface_ptr surf;
    element_id sts;
    double c, z;

    for (int j = 0; j < this->num_panels_y; ++j)
    {
        for (int i = 0; i < this->num_panels_x; ++i)
        {
            elem = make_element<SingleElement>();

            if (this->canting_method == NONE)
            {
                origin.set_values(panel_x, panel_y, 0.0);
                aim.set_values(panel_x, panel_y, 1.0);
            }
            else if (this->canting_method == ON_AXIS)
            {
                c = 0.5 / (this->onaxis_canting_distance);
                z = 0.5 * c * (panel_x * panel_x + panel_y * panel_y);
                origin.set_values(panel_x, panel_y, z);
                aim.set_values(0.0, 0.0, 2.0 * this->onaxis_canting_distance);
            }
            else if (this->canting_method == OFF_AXIS)
            {
                origin.set_values(panel_x, panel_y, 0.0);
                // TODO: Set aim vector values
                aim.set_values(0.0, 0.0, 1.0);
                throw std::runtime_error("OFF_AXIS is not yet implemented");
            }
            else if (this->canting_method == UNSET)
            {
                throw std::runtime_error("Canting method unset");
            }
            else
            {
                throw std::runtime_error("Unknown canting method");
            }

            elem->set_reference_frame_geometry(origin,
                                               aim,
                                               0.0);

            ap = make_aperture<Rectangle>(panel_len_x, panel_len_y);
            elem->set_aperture(ap);
            if (this->focal_length_x <= 0.0 && this->focal_length_y <= 0.0)
            {
                surf = make_surface<Flat>();
            }
            else
            {
                surf = make_surface<Parabola>(this->focal_length_x,
                                              this->focal_length_y);
            }
            elem->set_surface(surf);
            
            elem->set_front_optical_properties(this->optics_mirror);
            elem->set_back_optical_properties(this->optics_mirror);
            elem->enable();

            this->heliostat_area += panel_len_x * panel_len_y;
            this->facets.push_back(elem);
            sts = this->add_element(elem);
            if (!Element::is_success(sts))
            {
                throw std::runtime_error("Failed to add element");
            }

            panel_x += panel_len_x + this->gap_x;
        }
        panel_x = -0.5 * (this->aperture_size_x - panel_len_x);
        panel_y += panel_len_y + this->gap_y;
    }

    this->initialized = true;

    return;
}

void Heliostat::set_aperture_size(double size_x,
                                  double size_y)
{
    if (size_x <= 0.0 || size_y <= 0.0)
    {
        throw std::invalid_argument(
            "Aperture sizes must be strictly positive");
    }

    this->initialized = false;
    this->aperture_size_x = size_x;
    this->aperture_size_y = size_y;

    return;
}

void Heliostat::set_focal_length(double flen)
{
    if (flen < 0.0)
    {
        // TODO: Custom exception type here?
        throw std::invalid_argument(
            "Focal length must be nonnegative");
    }

    this->initialized = false;
    this->focal_length_x = flen;
    this->focal_length_y = flen;

    return;
}

void Heliostat::set_focal_length(double fx, double fy)
{
    if (fx < 0.0 || fy < 0.0)
    {
        throw std::invalid_argument(
            "Focal length must be nonnegative");
    }

    this->initialized = false;
    this->focal_length_x = fx;
    this->focal_length_y = fy;

    return;
}

void Heliostat::set_gaps(double gap_x,
                         double gap_y)
{
    if (gap_x < 0.0 || gap_y < 0.0)
    {
        throw std::invalid_argument(
            "All gap values must be nonnegative");
    }

    this->initialized = false;
    this->gap_x = gap_x;
    this->gap_y = gap_y;

    return;
}

void Heliostat::set_number_panels(uint_fast64_t num_x,
                                  uint_fast64_t num_y)
{
    if (num_x < 1 || num_y < 1)
    {
        throw std::invalid_argument(
            "Number of panels must be greater than one");
    }

    this->initialized = false;
    this->num_panels_x = num_x;
    this->num_panels_y = num_y;

    return;
}

void Heliostat::set_optics(const OpticalProperties &optics)
{
    this->optics_mirror = optics;
    return;
}

void Heliostat::set_canting(CantingType ct, double val1, double val2)
{
    this->initialized = false;
    this->canting_method = ct;

    if (ct == NONE)
    {
        ; // Intentional no-op
    }
    else if (ct == OFF_AXIS)
    {
        if (val1 < 0.0 || val1 > 360.0)
        {
            throw std::domain_error(
                "Off-axis canting sun azimuth angle be lie in [0, 360]");
        }
        if (val2 < 0.0 || val2 > 90.0)
        {
            throw std::domain_error(
                "Off-axis canting sun zenith angle must lie in [0, 90]");
        }
        this->offaxis_canting_sun_position_azimuth = val1;
        this->offaxis_canting_sun_position_zenith = val2;
        this->onaxis_canting_distance = -1.0;
    }
    else if (ct == ON_AXIS)
    {
        if (val1 <= 0.0)
        {
            throw std::domain_error(
                "On-axis canting distance must be strictly positive");
        }
        this->onaxis_canting_distance = val1;
        this->offaxis_canting_sun_position_azimuth = -1.0;
        this->offaxis_canting_sun_position_zenith = -1.0;
    }
    else
    {
        throw std::runtime_error("Unrecognized canting method");
    }

    return;
}