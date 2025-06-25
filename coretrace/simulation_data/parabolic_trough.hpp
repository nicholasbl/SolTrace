#ifndef SOLTRACE_PARABOLIC_TROUGH_H
#define SOLTRACE_PARABOLIC_TROUGH_H

#include <cstdint>

#include <vector>

// #include "aperture.hpp"
#include "composite_element.hpp"
#include "element.hpp"
#include "optical_properties.hpp"
#include "simulation_data.hpp"
#include "single_element.hpp"
// #include "surface.hpp"
#include "vector3d.hpp"

class ParabolicTrough : public CompositeElement
{
public:
    ParabolicTrough();
    ~ParabolicTrough();

    // void disable() { this->elements->disable(); }
    // void enable() { this->elements->enable(); }

    // element_id add_self(stage_ptr stage);
    // element_id add_self(SimulationData &sd);
    // void remove_self(SimulationData &sd);

    void create_geometry();
    void update_geometry(double solar_azimuth, double solar_elevation);

    double calculate_receiver_power();

    void set_angles(double azimuth, double tilt);
    void set_aperture_size(double size_x, double size_y);
    void set_focal_length(double flen);
    void set_gaps(double gap_x, double gap_y, double gap_center);
    void set_number_panels(uint_fast64_t num_x, uint_fast64_t num_y);
    void set_optics(const OpticalProperties &mirror,
                    const OpticalProperties &absorber,
                    const OpticalProperties &envelope_inner,
                    const OpticalProperties &envelope_outer);
    // void set_position(const Vector3d &pos)
    // {
    //     this->position = pos;
    //     return;
    // }
    void set_receiver_dimensions(double abs_diam,
                                 double env_diam,
                                 double env_thick);
    void set_tracking_limits(double lower, double upper);

    virtual void enforce_user_fields_set() const;

private:
    // Flags
    bool initialized;
    // bool ready_to_add_self;

    // Global Characteristics
    Vector3d track_axis;
    double azimuth;
    double tilt;

    // Reflector(s) Characteristic(s)
    double aperture_size_x;
    double aperture_size_y;
    double focal_length;
    double cx;
    double gap_x;
    double gap_y;
    double gap_center;
    uint_fast64_t num_panels_x;
    uint_fast64_t num_panels_y;
    OpticalProperties optics_mirror;

    // Receiver Characteristics
    double absorber_diameter;
    double envelope_diameter;
    double envelope_thickness;
    // double length;
    OpticalProperties optics_absorber;
    OpticalProperties optics_envelope_inner;
    OpticalProperties optics_envelope_outer;

    // Solar Tracking
    double tracking_limit_lower;
    double tracking_limit_upper;

    // Element Management
    // composite_element_ptr elements;
    std::vector<single_element_ptr> absorbers;
    std::vector<single_element_ptr> envelopes;
    std::vector<single_element_ptr> mirrors;

    double determine_x_coordinate(double x0, double arc_length);
};

#endif
