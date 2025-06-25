
#include "parabolic_trough.hpp"

#include <algorithm>
#include <stdexcept>

#include "aperture.hpp"
#include "arclength.hpp"
#include "composite_element.hpp"
#include "element.hpp"
#include "surface.hpp"

ParabolicTrough::ParabolicTrough()
    : initialized(false),
      //   ready_to_add_self(false),
      azimuth(0.0),
      tilt(0.0),
      aperture_size_x(1.0),
      aperture_size_y(1.0),
      focal_length(1.0),
      gap_x(0.0),
      gap_y(0.0),
      gap_center(0.0),
      num_panels_x(1),
      num_panels_y(1),
      absorber_diameter(0.1),
      envelope_diameter(0.25),
      envelope_thickness(0.05),
      //   length(1.0),
      tracking_limit_lower(0.0),
      tracking_limit_upper(180.0)
{
    // TODO: Initialize to nonsense and enforce user setting of values
    // TODO: Need to do something similar for elements totally
    return;
}

ParabolicTrough::~ParabolicTrough()
{
    absorbers.clear();
    envelopes.clear();
    mirrors.clear();
    return;
}

void ParabolicTrough::create_geometry()
{
    // TODO: Check that gap values are consistent with number of panels

    if (initialized)
        return;

    this->absorbers.clear();
    this->mirrors.clear();
    this->envelopes.clear();
    this->clear();

    // Mirrors
    aperture_ptr ap = nullptr;
    single_element_ptr panel = nullptr;
    surface_ptr surf = nullptr;
    Vector3d origin;
    Vector3d aim;
    double xstop;
    double gap;
    double ystart, ycoord;

    // double ystart = -0.5 * this->aperture_size_y;
    double panel_length_y = (this->aperture_size_y -
                             this->gap_y * (this->num_panels_y - 1)) /
                            this->num_panels_y;

    double hw = 0.5 * this->aperture_size_x;
    double xstart = -hw;
    double arc_length = hw * sqrt(hw * this->cx * hw * this->cx + 1) +
                        asinh(hw * this->cx) / this->cx;
    double panel_arc_length = arc_length -
                              this->gap_x * (this->num_panels_x - 1);
    panel_arc_length -= this->gap_center - this->gap_x;
    panel_arc_length /= this->num_panels_x;

    // NOTE: Integer (floor) division is intentional here.
    uint_fast64_t half_n_x = this->num_panels_x / 2;

    for (int i = 0; i < this->num_panels_x; ++i)
    {
        xstop = determine_x_coordinate(xstart, panel_arc_length);
        ystart = -0.5 * this->aperture_size_y;
        ycoord = ystart + 0.5 * panel_length_y;

        for (int j = 0; j < this->num_panels_y; ++j)
        {
            // std::cout << "**** i = " << i << "  j = " << j << " ****"
            //           << "\nxstart = " << xstart << "  xstop = " << xstop
            //           << "\nystart = " << ystart << "  ycoord = " << ycoord
            //           << "\npanel_len_y = " << panel_length_y
            //           << "\npanel_arc_len = " << panel_arc_length
            //           << "\narc_len = " << arc_length
            //           << std::endl;

            origin.set_values(0.0, ycoord, 0.0);
            aim.set_values(0.0, ycoord, 1.0);

            panel = make_element<SingleElement>();
            panel->set_name("ParabolicMirror");
            panel->set_reference_frame_geometry(origin, aim, 0.0);
            ap = make_aperture<Rectangle>(xstop - xstart,
                                          panel_length_y,
                                          xstart,
                                          ystart);
            panel->set_aperture(ap);
            surf = make_surface<Parabola>(this->focal_length, 0.0);
            panel->set_surface(surf);
            panel->set_front_optical_properties(this->optics_mirror);
            panel->enable();

            this->mirrors.push_back(panel);
            this->add_element(panel);

            ystart += panel_length_y + this->gap_y;
            ycoord = ystart + 0.5 * panel_length_y;
        }
        gap = i == this->num_panels_x / 2 - 1 ? this->gap_center : this->gap_x;
        // std::cout << "Gap: " << gap << std::endl;
        // std::cout << "this->num_panels_x / 2 = " << this->num_panels_x / 2
        //           << std::endl;
        xstart = determine_x_coordinate(xstop, gap);
    }

    // Absorber
    // TODO: Single element at the moment. Break up into multiple tubes.
    auto abs = make_element<SingleElement>();
    abs->set_name("Absorber");
    abs->set_origin(0.0, 0.0,
                    this->focal_length - 0.5 * this->absorber_diameter);
    abs->set_aim_vector(0.0, 0.0, 1.0);
    abs->set_zrot(0.0);
    abs->compute_coordinate_rotations();
    abs->set_aperture(make_aperture<Rectangle>(this->absorber_diameter,
                                               this->aperture_size_y));
    abs->set_surface(make_surface<Cylinder>(0.5 * this->absorber_diameter));
    abs->set_front_optical_properties(this->optics_absorber);
    abs->set_back_optical_properties(this->optics_absorber);
    abs->enable();
    this->absorbers.push_back(abs);
    auto id = this->add_element(abs);
    if (!Element::is_success(id))
    {
        // TODO: Make this more helpful
        throw std::runtime_error("Failed to add absorber element");
    }

    // Envelope -- Outer
    auto envout = make_element<SingleElement>();
    envout->set_name("EnvelopeOuter");
    envout->set_origin(0.0, 0.0,
                       this->focal_length - 0.5 * this->envelope_diameter);
    envout->set_aim_vector(0.0, 0.0, 1.0);
    envout->set_zrot(0.0);
    envout->compute_coordinate_rotations();
    envout->set_aperture(make_aperture<Rectangle>(this->envelope_diameter,
                                                  this->aperture_size_y));
    envout->set_surface(make_surface<Cylinder>(0.5 * this->envelope_diameter));
    envout->set_front_optical_properties(this->optics_envelope_outer);
    envout->set_back_optical_properties(this->optics_envelope_outer);
    envout->enable();
    this->envelopes.push_back(envout);
    id = this->add_element(envout);
    if (!Element::is_success(id))
    {
        throw std::runtime_error("Failed to add outer envelope element");
    }

    // Envelope -- Inner
    auto envin = make_element<SingleElement>();
    envin->set_name("EnvelopeInner");
    envin->set_origin(0.0, 0.0,
                      this->focal_length - 0.5 * this->envelope_diameter +
                          this->envelope_thickness);
    envin->set_aim_vector(0.0, 0.0, 1.0);
    envin->set_zrot(0.0);
    envin->compute_coordinate_rotations();
    double ap_x = this->envelope_diameter - 2 * this->envelope_thickness;
    double ap_y = this->aperture_size_y;
    envin->set_aperture(make_aperture<Rectangle>(ap_x, ap_y));
    envin->set_surface(make_surface<Cylinder>(0.5 * ap_x));
    envin->set_front_optical_properties(this->optics_envelope_inner);
    envin->set_back_optical_properties(this->optics_envelope_inner);
    envin->enable();
    this->envelopes.push_back(envin);
    id = this->add_element(envin);
    if (!Element::is_success(id))
    {
        throw std::runtime_error("Failed to add inner envelope element");
    }

    this->enable();
    this->initialized = true;

    return;
}

void ParabolicTrough::update_geometry(double solar_azimuth,
                                      double solar_elevetion)
{
    // TODO: Implement...
    return;
}

double ParabolicTrough::calculate_receiver_power()
{
    // TODO: Implement...
    return 0.0;
}

void ParabolicTrough::set_angles(double az, double tilt)
{
    if (az < -180.0 || az > 180)
    {
        throw std::invalid_argument(
            "Azimuth must be within -180 and 180 degrees");
    }

    if (tilt < 0.0 || tilt > 90.0)
    {
        throw std::invalid_argument(
            "Tilt angle should be between 0 and 90 degress");
    }

    // this->initialized = false;
    this->azimuth = az;
    this->tilt = tilt;

    return;
}

void ParabolicTrough::set_aperture_size(double size_x,
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

void ParabolicTrough::set_focal_length(double flen)
{
    if (flen <= 0.0)
    {
        // TODO: Custom exception type here?
        throw std::invalid_argument(
            "Focal length must be strictly positive");
    }

    this->initialized = false;
    this->focal_length = flen;
    this->cx = 0.5 / this->focal_length;

    return;
}

void ParabolicTrough::set_gaps(double gap_x,
                               double gap_y,
                               double gap_center)
{
    if (gap_x < 0.0 || gap_y < 0.0 || gap_center < 0.0)
    {
        throw std::invalid_argument(
            "All gap values must be nonnegative");
    }

    this->initialized = false;
    this->gap_x = gap_x;
    this->gap_y = gap_y;
    this->gap_center = gap_center;

    return;
}

void ParabolicTrough::set_number_panels(uint_fast64_t num_x,
                                        uint_fast64_t num_y)
{
    if (num_x < 1 || num_y < 1)
    {
        throw std::invalid_argument(
            "Number of panels must be greater than one");
    }

    if (num_x != 1 && num_x % 2 != 0)
    {
        throw std::invalid_argument(
            "Number of panels x direction must be either one or even");
    }

    this->initialized = false;
    this->num_panels_x = num_x;
    this->num_panels_y = num_y;

    return;
}

void ParabolicTrough::set_optics(const OpticalProperties &mirror,
                                 const OpticalProperties &absorber,
                                 const OpticalProperties &envelope_inner,
                                 const OpticalProperties &envelope_outer)
{
    this->optics_mirror = mirror;
    this->optics_absorber = absorber;
    this->optics_envelope_inner = envelope_inner;
    this->optics_envelope_outer = envelope_outer;
    return;
}

void ParabolicTrough::set_receiver_dimensions(double abs_diam,
                                              double env_diam,
                                              double env_thick)
{
    if (abs_diam <= 0.0)
    {
        throw std::invalid_argument(
            "Absorber diameter must be strictly positive");
    }

    if (env_diam <= 0.0)
    {
        throw std::invalid_argument(
            "Envelope diameter must be strictly positive");
    }

    if (env_thick <= 0.0)
    {
        throw std::invalid_argument(
            "Envelope thickness must be strictly positive");
    }

    // if (length <= 0.0)
    // {
    //     throw std::invalid_argument(
    //         "Receiver length must be strictly positive");
    // }

    if (env_diam - 2 * env_thick < abs_diam)
    {
        // TODO: Seems like this should be an error.
        throw std::invalid_argument(
            "Envelope is too small for the given absorber diameter");
    }

    this->initialized = false;

    return;
}

void ParabolicTrough::set_tracking_limits(double lower, double upper)
{
    if (lower > upper)
    {
        throw std::invalid_argument(
            "Upper tracking limit must be greater than lower limit");
    }

    this->tracking_limit_lower = lower;
    this->tracking_limit_upper = upper;

    return;
}

double ParabolicTrough::determine_x_coordinate(double x0,
                                               double arc_length)
{
    return parabolic_determine_x_coordinate(this->cx, x0, arc_length);
}

void ParabolicTrough::enforce_user_fields_set() const
{
    CompositeElement::enforce_user_fields_set();

    // TODO: Add required fields here

    return;
}