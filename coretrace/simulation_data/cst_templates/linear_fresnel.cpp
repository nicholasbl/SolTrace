#include "linear_fresnel.hpp"

#include <stdexcept>
#include <sstream>

#include "element.hpp"

LinearFresnel::LinearFresnel()
    : CompositeElement(),
      initialized(false),
      reciever_height(-1.0),
      azimuth(-1.0),
      tilt(-1.0),
      focused_panels(false),
      aperture_size_x(-1.0),
      aperture_size_y(-1.0),
      num_panels_x(-1),
      num_panels_y(-1),
      gap_x(-1.0),
      gap_y(-1.0),
      gap_center(-1.0),
      abs_diameter(-1.0),
      env_diameter(-1.0),
      env_thickness(-1.0)
{
    this->optics_absorber.set_ideal_absorption();
    this->optics_mirror.set_ideal_reflection();
    this->optics_env_out.set_ideal_transmission();
    this->optics_env_in.set_ideal_transmission();

    rotation_axis.set_values(0.0, 1.0, 0.0);
}

LinearFresnel::~LinearFresnel()
{
    // Clear vectors
    this->mirrors.clear();
    this->absorbers.clear();
    this->envelope.clear();

    return;
}

void LinearFresnel::set_angles(double azimuth, double tilt)
{
    if (azimuth < -180.0 || azimuth > 180.0)
    {
        std::stringstream ss;
        ss << "LinearFresnel::set_angles: Invalid azimuth angle ("
           << azimuth << "). Must be between -180 and 180 degrees.";
        throw std::invalid_argument(ss.str());
    }

    if (tilt < -90.0 || tilt > 90.0)
    {
        std::stringstream ss;
        ss << "LinearFresnel::set_angles: Invalid tilt angle ("
           << tilt << "). Must be between -90 and 90 degrees.";
        throw std::invalid_argument(ss.str());
    }

    this->azimuth = azimuth;
    this->tilt = tilt;

    double az = this->azimuth * M_PI / 180.0;
    double inc = this->tilt * M_PI / 180.0;

    this->rotation_axis.set_values(sin(inc) * cos(az),
                                   sin(inc) * sin(az),
                                   cos(az));

    return;
}

void LinearFresnel::set_aperture_size(double len_x, double len_y)
{
    if (len_x <= 0.0 || len_y <= 0.0)
    {
        std::stringstream ss;
        ss << "LinearFresnel::set_aperture_size: Invalid aperture dimensions. "
           << "len_x (" << len_x << ") and len_y (" << len_y
           << ") must be positive.";
        throw std::invalid_argument(ss.str());
    }

    this->initialized = false;
    this->aperture_size_x = len_x;
    this->aperture_size_y = len_y;

    return;
}

void LinearFresnel::set_focused_panels(bool focused)
{
    this->initialized = false;
    this->focused_panels = focused;
    return;
}

void LinearFresnel::set_gaps(double gap_x,
                             double gap_y,
                             double gap_center)
{
    if (gap_x < 0.0 || gap_y < 0.0 || gap_center < 0.0)
    {
        std::stringstream ss;
        ss << "LinearFresnel::set_gaps: Invalid gap dimensions. "
           << "gap_x (" << gap_x << "), gap_y (" << gap_y
           << "), and gap_center (" << gap_center
           << ") must be non-negative.";
        throw std::invalid_argument(ss.str());
    }

    this->initialized = false;
    this->gap_x = gap_x;
    this->gap_y = gap_y;
    this->gap_center = gap_center;

    return;
}

void LinearFresnel::set_number_panels(int_fast64_t num_x,
                                      int_fast64_t num_y)
{
    if (num_x <= 0 || num_y <= 0)
    {
        std::stringstream ss;
        ss << "LinearFresnel::set_number_panels: Invalid panel count. "
           << "num_x (" << num_x << ") and num_y (" << num_y
           << ") must be positive.";
        throw std::invalid_argument(ss.str());
    }

    this->initialized = false;
    this->num_panels_x = num_x;
    this->num_panels_y = num_y;

    return;
}

void LinearFresnel::set_optics(const OpticalProperties &mirror,
                               const OpticalProperties &absorber,
                               const OpticalProperties &envelop_outer,
                               const OpticalProperties &envelop_inner)
{
    this->optics_mirror = mirror;
    this->optics_absorber = absorber;
    this->optics_env_out = envelop_outer;
    this->optics_env_in = envelop_inner;
    return;
}

void LinearFresnel::set_receiver_height(double height)
{
    if (height <= 0.0)
    {
        std::stringstream ss;
        ss << "LinearFresnel::set_receiver_height: Invalid receiver height ("
           << height << "). Height must be positive.";
        throw std::invalid_argument(ss.str());
    }

    this->initialized = false;
    this->reciever_height = height;

    return;
}

void LinearFresnel::set_receiver_dimensions(double absorber_diameter,
                                            double envelop_diameter,
                                            double envelop_thickness)
{
    if (absorber_diameter <= 0.0)
    {
        std::stringstream ss;
        ss << "LinearFresnel::set_receiver_dimensions: "
           << "Invalid absorber diameter (" << absorber_diameter
           << "). Must be positive.";
        throw std::invalid_argument(ss.str());
    }

    if (envelop_diameter <= 0.0)
    {
        std::stringstream ss;
        ss << "LinearFresnel::set_receiver_dimensions: "
           << "Invalid envelope diameter (" << envelop_diameter
           << "). Must be positive.";
        throw std::invalid_argument(ss.str());
    }

    if (envelop_thickness < 0.0)
    {
        std::stringstream ss;
        ss << "LinearFresnel::set_receiver_dimensions: "
           << "Invalid envelope thickness (" << envelop_thickness
           << "). Must be non-negative.";
        throw std::invalid_argument(ss.str());
    }

    // if (length <= 0.0)
    // {
    //     std::stringstream ss;
    //     ss << "LinearFresnel::set_receiver_dimensions: "
    //        << "Invalid receiver length (" << length
    //        << "). Must be positive.";
    //     throw std::invalid_argument(ss.str());
    // }

    if (absorber_diameter >= envelop_diameter)
    {
        std::stringstream ss;
        ss << "LinearFresnel::set_receiver_dimensions: "
           << "Absorber diameter (" << absorber_diameter
           << ") must be smaller than envelope diameter ("
           << envelop_diameter << ").";
        throw std::invalid_argument(ss.str());
    }

    this->initialized = false;
    this->abs_diameter = absorber_diameter;
    this->env_diameter = envelop_diameter;
    this->env_thickness = envelop_thickness;

    return;
}

void LinearFresnel::create_geometry()
{
    if (this->initialized)
    {
        return;
    }

    this->enforce_user_fields_set();

    this->mirrors.clear();
    this->absorbers.clear();
    this->envelope.clear();
    this->clear();

    // Create mirrors
    if (this->num_panels_x == 1 && this->gap_center != 0.0)
    {
        // TODO: This should generate a warning.
        this->gap_center = 0.0;
        this->gap_x = 0.0;
    }

    double panel_len_x = this->aperture_size_x -
                         this->gap_x * (this->num_panels_x - 1);
    panel_len_x -= this->gap_center - this->gap_x;
    panel_len_x /= this->num_panels_x;
    double panel_x = -0.5 * this->aperture_size_x + 0.5 * panel_len_x;

    double panel_len_y = this->aperture_size_y -
                         this->gap_y * (this->num_panels_y - 1);
    panel_len_y /= this->num_panels_y;

    element_id sts;
    Vector3d origin;
    Vector3d aim;
    // TODO: Should mirrors aim at the receiver center or origin?
    Vector3d receiver_pos(0.0,
                          0.0,
                          this->reciever_height - 0.5 * this->abs_diameter);
    double panel_y, flen;
    single_element_ptr mirror;
    aperture_ptr ap;
    surface_ptr surf;
    Vector3d khat(0.0, 0.0, 1.0);

    for (int_fast64_t i = 0; i < this->num_panels_x; ++i)
    {
        panel_y = -0.5 * this->aperture_size_y + 0.5 * panel_len_y;
        for (int_fast64_t j = 0; j < this->num_panels_y; ++j)
        {
            mirror = make_element<SingleElement>();

            origin.set_values(panel_x, panel_y, 0.0);
            vector_add(1.0, receiver_pos, -1.0, origin, aim);
            vector_add(0.5, khat, 0.5, aim);
            vector_add(1.0, origin, 1.0, aim);
            mirror->set_reference_frame_geometry(origin, aim, 0.0);

            ap = make_aperture<Rectangle>(panel_len_x, panel_len_y);
            mirror->set_aperture(ap);

            if (this->focused_panels)
            {
                flen = sqrt(panel_x * panel_x +
                            this->reciever_height * this->reciever_height);
                surf = make_surface<Parabola>(flen, 0.0);
            }
            else
            {
                surf = make_surface<Flat>();
            }
            mirror->set_surface(surf);

            mirror->set_front_optical_properties(this->optics_mirror);
            mirror->set_back_optical_properties(this->optics_mirror);
            mirror->enable();

            std::stringstream name;
            name << "Mirror_" << this->num_panels_y * i + j;
            mirror->set_name(name.str());

            this->mirrors.push_back(mirror);
            sts = this->add_element(mirror);
            if (!Element::is_success(sts))
            {
                // TODO: Make this more helpful
                throw std::runtime_error("Failed to add mirror element");
            }

            panel_y += panel_len_y + this->gap_y;
        }
        panel_x += panel_len_x;
        if (i == this->num_panels_x / 2 - 1)
        {
            panel_x += this->gap_center;
        }
        else
        {
            panel_x += this->gap_x;
        }
    }

    // Absorber
    // TODO: Single element at the moment. Break up into multiple tubes.
    auto abs = make_element<SingleElement>();
    abs->set_name("Absorber");
    origin.set_values(0.0, 0.0,
                      this->reciever_height - 0.5 * this->abs_diameter);
    aim.set_values(0.0, 0.0, 1.0);
    vector_add(1.0, origin, 1.0, aim);
    abs->set_reference_frame_geometry(origin, aim, 0.0);
    abs->set_aperture(make_aperture<Rectangle>(this->abs_diameter,
                                               this->aperture_size_y));
    abs->set_surface(make_surface<Cylinder>(0.5 * this->abs_diameter));
    abs->set_front_optical_properties(this->optics_absorber);
    abs->set_back_optical_properties(this->optics_absorber);
    abs->enable();

    this->absorbers.push_back(abs);
    sts = this->add_element(abs);
    if (!Element::is_success(sts))
    {
        // TODO: Make this more helpful
        throw std::runtime_error("Failed to add absorber element");
    }

    // Envelope -- Outer
    auto envout = make_element<SingleElement>();
    envout->set_name("EnvelopeOuter");
    origin.set_values(0.0, 0.0,
                      this->reciever_height - 0.5 * this->env_diameter);
    aim.set_values(0.0, 0.0, 1.0);
    vector_add(1.0, origin, 1.0, aim);
    envout->set_reference_frame_geometry(origin, aim, 0.0);
    envout->set_aperture(make_aperture<Rectangle>(this->env_diameter,
                                                  this->aperture_size_y));
    envout->set_surface(make_surface<Cylinder>(0.5 * this->env_diameter));
    envout->set_front_optical_properties(this->optics_env_out);
    envout->set_back_optical_properties(this->optics_env_out);
    envout->enable();

    this->envelope.push_back(envout);
    sts = this->add_element(envout);
    if (!Element::is_success(sts))
    {
        throw std::runtime_error("Failed to add outer envelope element");
    }

    // Envelope -- Inner
    auto envin = make_element<SingleElement>();
    envin->set_name("EnvelopeInner");
    origin.set_values(0.0, 0.0,
                      this->reciever_height -
                          0.5 * this->env_diameter + this->env_thickness);
    aim.set_values(0.0, 0.0, 1.0);
    vector_add(1.0, origin, 1.0, aim);
    envin->set_reference_frame_geometry(origin, aim, 0.0);
    double ap_x = this->env_diameter - 2 * this->env_thickness;
    double ap_y = this->aperture_size_y;
    envin->set_aperture(make_aperture<Rectangle>(ap_x, ap_y));
    envin->set_surface(make_surface<Cylinder>(0.5 * ap_x));
    envin->set_front_optical_properties(this->optics_env_in);
    envin->set_back_optical_properties(this->optics_env_in);
    envin->enable();
    this->envelope.push_back(envin);
    sts = this->add_element(envin);
    if (!Element::is_success(sts))
    {
        throw std::runtime_error("Failed to add inner envelope element");
    }

    this->initialized = true;
    return;
}

void LinearFresnel::update_geometry()
{
    // TODO: Implement geometry update for linear Fresnel system
    // This should update existing geometry based on current parameters
    if (this->initialized)
    {
        // Update existing geometry
    }
    else
    {
        // Create geometry if not initialized
        this->create_geometry();
        // TODO: Add some aim settings here...
    }

    return;
}

void LinearFresnel::enforce_user_fields_set() const
{
    if (this->initialized)
    {
        CompositeElement::enforce_user_fields_set();
    }

    // Validate that all required parameters have been set
    if (this->aperture_size_x <= 0.0 || this->aperture_size_y <= 0.0)
    {
        throw std::invalid_argument(
            "LinearFresnel: Aperture size must be set "
            "before creating geometry.");
    }

    if (this->num_panels_x <= 0 || this->num_panels_y <= 0)
    {
        throw std::invalid_argument(
            "LinearFresnel: Number of panels must be set "
            "before creating geometry.");
    }

    if (this->reciever_height <= 0.0)
    {
        throw std::invalid_argument(
            "LinearFresnel: Receiver height must be set "
            "before creating geometry.");
    }

    if (this->abs_diameter <= 0.0 ||
        this->env_diameter <= 0.0)
    {
        throw std::invalid_argument(
            "LinearFresnel: Receiver dimensions must be set "
            "before creating geometry.");
    }

    return;
}
