#ifndef SOLTRACE_UNITTESTS_COMMON_H
#define SOLTRACE_UNITTESTS_COMMON_H

#include <element.hpp>
#include <simulation_data_export.hpp>
#include <vector3d.hpp>

// Vectors exactly match component-wise
bool is_identical(const SolTrace::Data::Vector3d &x,
                  const SolTrace::Data::Vector3d &y);
// Each vector component are within `tol` of each other so ||x - y||_\infty <= tol
bool is_identical(const SolTrace::Data::Vector3d &x,
                  const SolTrace::Data::Vector3d &y,
                  double tol);
bool is_identical(const SolTrace::Data::Matrix3d &A,
                  const SolTrace::Data::Matrix3d &B);

// Convenience function for making element with all
// required fields are set. Used when the test does not
// care about the specifics of the element.
SolTrace::Data::element_ptr make_configured_element();

#endif
