#ifndef SOLTRACE_HELIOSTAT_H
#define SOLTRACE_HELIOSTAT_H

#include <vector>

#include "composite_element.hpp"
#include "single_element.hpp"

class Heliostat : public CompositeElement
{
public:
    enum CantingType
    {
        UNSET = 0,
        NONE = 1,
        OFF_AXIS = 2,
        ON_AXIS = 3
    };

    Heliostat();
    ~Heliostat();

    void create_geometry();
    void update_geometry(double solar_azimuth, double solar_elevation);

    void set_aperture_size(double size_x, double size_y);
    void set_focal_length(double flen);
    void set_focal_length(double fx, double fy);
    void set_gaps(double gap_x, double gap_y);
    void set_number_panels(uint_fast64_t num_x, uint_fast64_t num_y);
    void set_optics(const OpticalProperties &optics);
    // void set_onaxis_canting_distance(double dist);
    // void set_offaxis_canting_sun_position(double azimuth, double zenith);
    void set_canting(CantingType ct, double val1, double val2);

private:
    bool initialized;

    double aperture_size_x;
    double aperture_size_y;
    double focal_length_x;
    double focal_length_y;
    // double cx;
    double gap_x;
    double gap_y;

    uint_fast64_t num_panels_x;
    uint_fast64_t num_panels_y;
    OpticalProperties optics_mirror;

    CantingType canting_method;
    double onaxis_canting_distance;
    double offaxis_canting_sun_position_azimuth;
    double offaxis_canting_sun_position_zenith;

    double heliostat_area;
    double tracking_azimuth;
    double tracking_elevation;

    std::vector<single_element_ptr> facets;
};

#endif
