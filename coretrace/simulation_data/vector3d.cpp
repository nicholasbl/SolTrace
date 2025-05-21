
#include "vector3d.hpp"

// #include <cassert>

#include <cmath>

#include "matvec.hpp"

// inline void vector_copy(double data[3], const Vector3d &x)
// {
//     CopyVec3(data, x.data);
//     return;
// }
// inline void vector_copy(std::vector<double> &dest, const Vector3d &x)
// {
//     CopyVec3(dest, x.data);
//     return;
// }

// Compute y = A*x placing the result in y
void matrix_vector_product(const Matrix3d &A, const Vector3d &x, Vector3d &y)
{
    MatrixVectorMult(A.data, x.data, y.data);
    return;
}

void matrix_matrix_product(const Matrix3d &A, const Matrix3d &B, Matrix3d &C)
{
    MatrixMatrixMult(A.data, B.data, C.data);
    return;
}

void vector_add(double a, const Vector3d &x,
                double b, const Vector3d &y,
                Vector3d &z)
{
    for (int i = 0; i < 3; ++i)
    {
        z[i] = a * x[i] + b * y[i];
    }
    return;
}

void vector_max(const Vector3d &x, const Vector3d &y, Vector3d &max)
{
    for (int i = 0; i < 3; ++i)
    {
        max[i] = fmax(x[i], y[i]);
    }
    return;
}

void vector_min(const Vector3d &x, const Vector3d &y, Vector3d &min)
{
    for (int i = 0; i < 3; ++i)
    {
        min[i] = fmin(x[i], y[i]);
    }
    return;
}

// Compute standard Euclidean dot product
double dot_product(const Vector3d &x, const Vector3d &y)
{
    return DOT(x.data, y.data);
}

double vector_norm(const Vector3d &x)
{
    return sqrt(DOT(x.data, x.data));
}

void make_unit_vector(Vector3d &x)
{
    double mag = vector_norm(x);
    assert(mag > 0.0);
    for (int i = 0; i < 3; ++i)
    {
        x.data[i] /= mag;
    }
    return;
}

void transform_to_local(Vector3d &pos_ref,
                        Vector3d &cos_ref,
                        Vector3d &origin,
                        Matrix3d &ref_to_local,
                        Vector3d &pos_local,
                        Vector3d &cos_local)
{
    TransformToLocal(pos_ref.data, cos_ref.data,
                     origin.data, ref_to_local.data,
                     pos_local.data, cos_local.data);
    return;
}

void transform_to_reference(Vector3d &pos_local,
                            Vector3d &cos_local,
                            Vector3d &origin,
                            Matrix3d &local_to_ref,
                            Vector3d &pos_ref,
                            Vector3d &cos_ref)
{
    TransformToReference(pos_local.data, cos_local.data,
                         origin.data, local_to_ref.data,
                         pos_ref.data, cos_ref.data);
    return;
}

void compute_transform_matrices(Vector3d &euler,
                                Matrix3d &ref_to_local,
                                Matrix3d &local_to_ref)
{
    CalculateTransformMatrices(euler.data,
                               ref_to_local.data,
                               local_to_ref.data);
    return;
}
