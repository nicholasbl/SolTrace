/**
 * @file parabolic_dish.hpp
 * @brief Parabolic dish CST template for dish-Stirling systems
 *
 * Defines the ParabolicDish class which generates parabolic dish
 * concentrator systems with configurable panel layouts, focal receivers,
 * and tracking capabilities for dish-Stirling solar applications.
 * Supports radial and angular panel subdivision strategies.
 *
 * @defgroup cst_templates CST Templates
 * @{
 */

#ifndef SOLTRACE_PARABOLIC_DISH_H
#define SOLTRACE_PARABOLIC_DISH_H

#include "composite_element.hpp"

#include <vector>

#include "single_element.hpp"
#include "vector3d.hpp"

class ParabolicDish : public CompositeElement
{
public:
    ParabolicDish();
    ~ParabolicDish();

    void set_aperture_size(double diameter);
    void set_focal_length(double flen);
    void set_gaps(double radial, double angular, double center_radius);
    void set_number_of_panels(int_fast64_t nradial, int_fast64_t nangular);
    void set_optics(const OpticalProperties &mirror,
                    const OpticalProperties &absorber);
    void set_receiver_dimensions(double diameter, double distance);

    void create_geometry();
    void update_geometry();

    virtual void enforce_user_fields_set() const;

private:
    bool initialized;

    // Reflector Characteristics
    double aperture_diameter;
    double aperture_radius;
    double focal_length;
    double cx;
    double gap_r;
    double gap_a;
    double gap_center;
    int_fast64_t num_panels_r;
    int_fast64_t num_panels_a;
    OpticalProperties optics_mirror;

    // Receiver Characteristics
    double abs_diameter;
    double abs_distance;
    OpticalProperties optics_absorber;

    double tracking_elevation;
    double tracking_azimuth;
    Vector3d elevation_axis;

    std::vector<single_element_ptr> mirrors;
    std::vector<single_element_ptr> absorbers;

    double determine_x_coordinate(double x0, double arc_length);

};

/**
 * @}
 */

#endif
