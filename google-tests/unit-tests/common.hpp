#ifndef SOLTRACE_UNITTESTS_COMMON_H
#define SOLTRACE_UNITTESTS_COMMON_H

#include <vector3d.hpp>

// Vectors exactly match component-wise
bool is_identical(const Vector3d &x, const Vector3d &y);
// Each vector component are within `tol` of each other so ||x - y||_\infty <= tol
bool is_identical(const Vector3d &x, const Vector3d &y, double tol);
bool is_identical(const Matrix3d &A, const Matrix3d &B);

#endif
