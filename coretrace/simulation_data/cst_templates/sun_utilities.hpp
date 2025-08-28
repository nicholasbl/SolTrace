
#ifndef SOLTRACE_SUN_UTILITIES_H
#define SOLTRACE_SUN_UTILITIES_H

#include "vector3d.hpp"

#ifndef M_PI
#define M_PI 3.141592653589793238462643
#endif

void sun_position_vector_degrees(Vector3d &sun_pos,
                                 double azimuth,
                                 double elevation);

void sun_position_vector_radians(Vector3d &sun_pos,
                                 double azimuth,
                                 double elevation);

#endif
