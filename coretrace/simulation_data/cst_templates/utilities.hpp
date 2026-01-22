
#ifndef SOLTRACE_UTILITIES_H
#define SOLTRACE_UTILITIES_H

#include <glm/vec3.hpp>

namespace SolTrace::Data {

void project_onto_plane(const glm::dvec3 &n,
                        const glm::dvec3 &u,
                        glm::dvec3 &uproj);

void project_onto_plane(const glm::dvec3 &n,
                        glm::dvec3 &u);

void project_onto_vector(const glm::dvec3 &u,
                         const glm::dvec3 &v,
                         glm::dvec3 &vproj);

void project_onto_vector(const glm::dvec3 &u,
                         glm::dvec3 &v);

void rotate_vector_degrees(const glm::dvec3 &k,
                           const glm::dvec3 &v,
                           double theta,
                           glm::dvec3 &vrot);

void rotate_vector_radians(const glm::dvec3 &k,
                           const glm::dvec3 &v,
                           double theta,
                           glm::dvec3 &vrot);

void sun_position_vector_degrees(glm::dvec3 &sun_pos,
                                 double azimuth,
                                 double elevation);

void sun_position_vector_radians(glm::dvec3 &sun_pos,
                                 double azimuth,
                                 double elevation);

} // namespace SolTrace::Data

#endif
