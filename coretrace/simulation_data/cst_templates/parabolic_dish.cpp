
#include "parabolic_dish.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <stdexcept>

#include "arclength.hpp"

ParabolicDish::ParabolicDish() : CompositeElement(),
                                 initialized(false),
                                 aperture_diameter(-1.0),
                                 aperture_radius(-1.0),
                                 focal_length(-1.0),
                                 cx(-1.0),
                                 gap_r(-1.0),
                                 gap_a(-1.0),
                                 gap_center(-1.0),
                                 num_panels_r(-1),
                                 num_panels_a(-1),
                                 abs_diameter(-1.0),
                                 abs_distance(-1.0),
                                 tracking_elevation(-1.0),
                                 tracking_azimuth(-1.0)
{
}

ParabolicDish::~ParabolicDish()
{
    this->absorbers.clear();
    this->mirrors.clear();
    return;
}

void ParabolicDish::create_geometry()
{
    if (this->initialized)
        return;

    this->enforce_user_fields_set();

    // TODO: What is the correct behavior when gap_center is unset (i.e. < 0)?
    // Currently, if gap_center is unset, then there is no gap at the center.

    this->absorbers.clear();
    this->mirrors.clear();
    this->clear();

    /**** Create mirror elements ****/
    double r = this->aperture_radius;
    double cx = this->cx;
    double arc_length = r * sqrt(cx * r * cx * r) + asinh(r * cx) / cx;
    double panel_arc_length = 0.5 * arc_length -
                              this->gap_r * (this->num_panels_r - 1);

    if (this->gap_center > 0.0)
    {
        double center_arc_length =
            parabolic_arc_length(cx, 0.0, this->gap_center);
        panel_arc_length -= center_arc_length;
    }

    panel_arc_length /= this->num_panels_r;

    double gap_angle = 0.0;
    double panel_angle = 360.0;
    if (this->num_panels_a > 1)
    {
        // this->gap_a gives the gap as a length -- convert to angle
        // so that the average gap is the value this->gap_a.
        gap_angle = this->gap_a * 360.0 / (M_PI * this->aperture_radius);
        panel_angle = 360.0 - gap_angle * this->num_panels_a;
        panel_angle /= this->num_panels_a;
    }

    single_element_ptr mirror;
    Vector3d origin(0.0, 0.0, 0.0);
    Vector3d aim(0.0, 0.0, 100.0);
    double zrot = 0.0;

    if (this->gap_center <= 0.0 &&
        this->num_panels_r == 1 &&
        this->num_panels_a == 1)
    {
        mirror = make_element<SingleElement>();
        mirror->set_reference_frame_geometry(origin, aim, zrot);
        mirror->set_aperture(make_aperture<Circle>(this->aperture_diameter));
        mirror->set_surface(make_surface<Parabola>(this->focal_length,
                                                   this->focal_length));
        mirror->set_front_optical_properties(this->optics_mirror);
        mirror->set_back_optical_properties(this->optics_mirror);
        mirror->enable();

        this->mirrors.push_back(mirror);
        this->add_element(mirror);
    }
    else
    {
        double panel_zrot_offset = panel_angle + gap_angle;
        double r0 = std::max(0.0, this->gap_center);
        double r1;

        for (int_fast64_t i = 0; i < this->num_panels_r; ++i)
        {
            r1 = this->determine_x_coordinate(r0, panel_arc_length);
            zrot = 0.0;
            for (int_fast64_t j = 0; j < this->num_panels_a; ++j)
            {
                mirror = make_element<SingleElement>();
                mirror->set_reference_frame_geometry(origin, aim, zrot);
                mirror->set_aperture(make_aperture<Annulus>(r0,
                                                            r1,
                                                            panel_angle));
                mirror->set_surface(make_surface<Parabola>(this->focal_length,
                                                           this->focal_length));
                mirror->set_front_optical_properties(this->optics_mirror);
                mirror->set_back_optical_properties(this->optics_mirror);
                mirror->set_name("ParabolicMirror");
                mirror->enable();

                this->mirrors.push_back(mirror);
                this->add_element(mirror);

                zrot += panel_zrot_offset;
            }
            r0 = this->determine_x_coordinate(r1, this->gap_r);
        }
    }

    /**** Create absorber element(s) ****/
    single_element_ptr abs = make_element<SingleElement>();
    origin.set_values(0.0, 0.0, this->abs_distance);
    aim.set_values(0.0, 0.0, this->abs_distance - 1.0);
    abs->set_reference_frame_geometry(origin, aim, 0.0);
    abs->set_aperture(make_aperture<Circle>(this->abs_diameter));
    abs->set_surface(make_surface<Flat>());
    abs->set_front_optical_properties(this->optics_absorber);
    abs->set_back_optical_properties(this->optics_absorber);
    abs->set_name("Absorber");
    abs->enable();

    this->absorbers.push_back(abs);
    this->add_element(abs);

    this->initialized = true;

    return;
}

void ParabolicDish::update_geometry()
{
    // TODO: Implment this
    return;
}

void ParabolicDish::set_aperture_size(double diameter)
{
    if (diameter <= 0.0)
    {
        std::stringstream ss;
        ss << "ParabolicDish::set_aperture_size: Invalid aperture diameter ("
           << diameter << "). Must be positive.";
        throw std::invalid_argument(ss.str());
    }

    this->initialized = false;
    this->aperture_radius = 0.5 * diameter;
    this->aperture_diameter = diameter;

    return;
}

void ParabolicDish::set_focal_length(double flen)
{
    if (flen <= 0.0)
    {
        std::stringstream ss;
        ss << "ParabolicDish::set_focal_length: Invalid focal length ("
           << flen << "). Must be positive.";
        throw std::invalid_argument(ss.str());
    }

    this->initialized = false;
    this->focal_length = flen;
    this->cx = 0.5 / this->focal_length;

    return;
}

void ParabolicDish::set_gaps(double radial,
                             double angular,
                             double center_radius)
{
    if (radial < 0.0)
    {
        std::stringstream ss;
        ss << "ParabolicDish::set_gaps: Invalid radial gap ("
           << radial << "). Must be non-negative.";
        throw std::invalid_argument(ss.str());
    }

    if (angular < 0.0)
    {
        std::stringstream ss;
        ss << "ParabolicDish::set_gaps: Invalid angular gap ("
           << angular << "). Must be non-negative.";
        throw std::invalid_argument(ss.str());
    }

    // Note: center_radius can be negative (meaning no center gap)
    this->initialized = false;
    this->gap_r = radial;
    this->gap_a = angular;
    this->gap_center = center_radius;

    return;
}

void ParabolicDish::set_number_of_panels(int_fast64_t nradial,
                                         int_fast64_t nangular)
{
    if (nradial < 1)
    {
        std::stringstream ss;
        ss << "ParabolicDish::set_number_of_panels: Invalid number of "
           << "radial panels (" << nradial << "). Must be at least 1.";
        throw std::invalid_argument(ss.str());
    }

    if (nangular < 1)
    {
        std::stringstream ss;
        ss << "ParabolicDish::set_number_of_panels: Invalid number of "
           << "angular panels (" << nangular << "). Must be at least 1.";
        throw std::invalid_argument(ss.str());
    }

    this->initialized = false;
    this->num_panels_r = nradial;
    this->num_panels_a = nangular;

    return;
}

void ParabolicDish::set_optics(const OpticalProperties &mirror,
                               const OpticalProperties &absorber)
{
    this->optics_mirror = mirror;
    this->optics_absorber = absorber;
    return;
}

void ParabolicDish::set_receiver_dimensions(double diameter,
                                            double distance)
{
    if (diameter <= 0.0)
    {
        std::stringstream ss;
        ss << "ParabolicDish::set_receiver_dimensions: "
           << "Invalid receiver diameter (" << diameter
           << "). Must be positive.";
        throw std::invalid_argument(ss.str());
    }

    if (distance < 0.0)
    {
        std::stringstream ss;
        ss << "ParabolicDish::set_receiver_dimensions: "
           << "Invalid receiver distance (" << distance
           << "). Must be non-negative.";
        throw std::invalid_argument(ss.str());
    }

    this->initialized = false;
    this->abs_diameter = diameter;
    this->abs_distance = distance;

    return;
}

double ParabolicDish::determine_x_coordinate(double x0,
                                             double arc_length)
{
    return parabolic_determine_x_coordinate(this->cx, x0, arc_length);
}

void ParabolicDish::enforce_user_fields_set() const
{
    if (this->initialized)
    {
        CompositeElement::enforce_user_fields_set();
    }

    // Validate that all required parameters have been set
    if (this->aperture_diameter <= 0.0)
    {
        throw std::invalid_argument(
            "ParabolicDish: Aperture diameter must be set "
            "before creating geometry.");
    }

    if (this->focal_length <= 0.0)
    {
        throw std::invalid_argument(
            "ParabolicDish: Focal length must be set "
            "before creating geometry.");
    }

    if (this->num_panels_r <= 0 || this->num_panels_a <= 0)
    {
        throw std::invalid_argument(
            "ParabolicDish: Number of panels must be set "
            "before creating geometry.");
    }

    if (this->abs_diameter <= 0.0 || this->abs_distance < 0.0)
    {
        throw std::invalid_argument(
            "ParabolicDish: Receiver dimensions must be set "
            "before creating geometry.");
    }

    return;
}
