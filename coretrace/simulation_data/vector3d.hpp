#ifndef SOLTRACE_VECTOR3D_H
#define SOLTRACE_VECTOR3D_H

#include <cassert>
#include <vector>

#include "matvec.hpp"

class Vector3d
{
public:
    Vector3d();
    Vector3d(double data[3]);
    Vector3d(double x, double y, double z);
    ~Vector3d();

    void zero();
    void set_values(double x, double y, double z);

    const double &operator[](int idx) const;
    double &operator[](int idx);
    friend std::ostream &operator<<(std::ostream &os, const Vector3d &x);

    double data[3];

private:
};

class Matrix3d
{
public:
    Matrix3d();
    ~Matrix3d();
    void set_value(int i, int j, double val);
    double get_value(int i, int j) const;

    void zero();
    void identity();

    // double data[9];
    double data[3][3];

    friend std::ostream &operator<<(std::ostream &os, const Matrix3d &A);

private:
};

inline void vector_copy(double data[3], const Vector3d &x)
{
    CopyVec3(data, x.data);
    return;
}
inline void vector_copy(std::vector<double> &dest, const Vector3d &x)
{
    CopyVec3(dest, x.data);
    return;
}

inline void matrix_copy(double data[3][3], const Matrix3d &A)
{
    CopyMat3(data, A.data);
    return;
}

// Compute y = A*x placing the result in y
void matrix_vector_product(const Matrix3d &A, const Vector3d &x, Vector3d &y);
// Compute C = A * B placing result in C
void matrix_matrix_product(const Matrix3d &A, const Matrix3d &B, Matrix3d &C);

// Compute z = a*x + b*y (result stored in z)
void vector_add(double a, const Vector3d &x,
                double b, const Vector3d &y,
                Vector3d &z);
// Compute y = a*x + b*y (result stored in y)
void vector_add(double a, const Vector3d &x,
                double b, Vector3d &y);

void vector_max(const Vector3d &x, const Vector3d &y, Vector3d &max);

void vector_min(const Vector3d &x, const Vector3d &y, Vector3d &min);

// Compute standard Euclidean dot product
double dot_product(const Vector3d &x, const Vector3d &y);
double vector_norm(const Vector3d &x);
void make_unit_vector(Vector3d &x);
// void transform_to_local(Vector3d &pos_ref,
//                         Vector3d &cos_ref,
//                         Vector3d &origin,
//                         Matrix3d &ref_to_local,
//                         Vector3d &pos_local,
//                         Vector3d &cos_local);
// void transform_to_reference(Vector3d &pos_local,
//                             Vector3d &cos_local,
//                             Vector3d &origin,
//                             Matrix3d &local_to_ref,
//                             Vector3d &pos_ref,
//                             Vector3d &cos_ref);
void compute_transform_matrices(Vector3d &euler,
                                Matrix3d &ref_to_local,
                                Matrix3d &local_to_ref);

#endif
