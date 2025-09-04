
#ifndef SOLTRACE_UTILITIES_H
#define SOLTRACE_UTILITIES_H

#include "vector3d.hpp"

// #ifndef M_PI
// #define M_PI 3.141592653589793238462643
// #endif

void project_onto_plane(const Vector3d &n,
                        const Vector3d &u,
                        Vector3d &uproj);

void project_onto_plane(const Vector3d &n,
                        Vector3d &u);

void project_onto_vector(const Vector3d &u,
                         const Vector3d &v,
                         Vector3d &vproj);

void project_onto_vector(const Vector3d &u,
                         Vector3d &v);

void rotate_vector_degrees(const Vector3d &k,
                           const Vector3d &v,
                           double theta,
                           Vector3d &vrot);

void rotate_vector_radians(const Vector3d &k,
                           const Vector3d &v,
                           double theta,
                           Vector3d &vrot);

void sun_position_vector_degrees(Vector3d &sun_pos,
                                 double azimuth,
                                 double elevation);

void sun_position_vector_radians(Vector3d &sun_pos,
                                 double azimuth,
                                 double elevation);

#endif
