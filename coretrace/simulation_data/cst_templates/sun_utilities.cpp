
#include "sun_utilities.hpp"

#include <cmath>

#include "vector3d.hpp"

void sun_position_vector_degrees(Vector3d &sun_pos,
                                 double azimuth,
                                 double elevation)
{
    double D2R = M_PI / 180;
    return sun_position_vector_radians(sun_pos,
                                       D2R * azimuth,
                                       D2R * elevation);
}

void sun_position_vector_radians(Vector3d &sun_pos,
                                 double azimuth,
                                 double elevation)
{
    double x = sin(azimuth) * cos(elevation);
    double y = cos(azimuth) * cos(elevation);
    double z = sin(elevation);
    sun_pos.set_values(x, y, z);
    return;
}
