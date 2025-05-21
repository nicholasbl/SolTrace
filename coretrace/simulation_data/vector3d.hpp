#ifndef SOLTRACE_VECTOR3D_H
#define SOLTRACE_VECTOR3D_H

#include <cassert>
#include <vector>

#include "matvec.hpp"

class Vector3d
{
public:
    Vector3d()
    {
        this->zero();
    }
    Vector3d(double data[3])
    {
        for (int i = 0; i < 3; ++i)
            this->data[i] = data[i];
    }
    Vector3d(double x, double y, double z)
    {
        this->data[0] = x;
        this->data[1] = y;
        this->data[2] = z;
    }
    ~Vector3d() {}

    void zero()
    {
        for (int i = 0; i < 3; ++i)
            this->data[i] = 0.0;
    }

    // inline double get_value(int i) const
    // {
    //     assert(i >= 0 && i < 3);
    //     return this->data[i];
    // }

    // void set_value(int i, double val)
    // {
    //     assert(i >= 0 && i < 3);
    //     this->data[i] = val;
    //     return;
    // }

    void set_values(double x, double y, double z)
    {
        this->data[0] = x;
        this->data[1] = y;
        this->data[2] = z;
        return;
    }

    inline const double &operator[](int idx) const
    {
        assert(idx >= 0 && idx < 3);
        return this->data[idx];
    }

    inline double &operator[](int idx)
    {
        assert(idx >= 0 && idx < 3);
        return this->data[idx];
    }

    // inline bool operator==(const Vector3d& x)
    // {
    //     return (
    //         data[0] == x.data[0] &&
    //         data[1] == x.data[1] &&
    //         data[2] == x.data[2]
    //     );
    // }

    double data[3];

private:
};

class Matrix3d
{
public:
    Matrix3d()
    {
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                this->data[i][j] = 0.0;
    }
    ~Matrix3d() {}

    void set_value(int i, int j, double val)
    {
        assert(i >= 0 && i < 3);
        assert(j >= 0 && j < 3);
        this->data[i][j] = val;
    }

    double get_value(int i, int j) const
    {
        assert(i >= 0 && i < 3);
        assert(j >= 0 && j < 3);
        return this->data[i][j];
    }

    void zero()
    {
        for (int i=0; i<3; ++i)
        {
            for (int j=0; j<3; ++j)
            {
                data[i][j] = 0.0;
            }
        }
    }

    // double data[9];
    double data[3][3];

private:
};

// inline void vector_copy(double data[3], const Vector3d &x);
// inline void vector_copy(std::vector<double> &dest, const Vector3d &x);
// inline void vector_copy();

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

void vector_add(double a, const Vector3d &x,
                double b, const Vector3d &y,
                Vector3d &z);

void vector_max(const Vector3d &x, const Vector3d &y, Vector3d &max);

void vector_min(const Vector3d &x, const Vector3d &y, Vector3d &min);

// Compute standard Euclidean dot product
double dot_product(const Vector3d &x, const Vector3d &y);
double vector_norm(const Vector3d &x);
void make_unit_vector(Vector3d &x);
void transform_to_local(Vector3d &pos_ref,
                        Vector3d &cos_ref,
                        Vector3d &origin,
                        Matrix3d &ref_to_local,
                        Vector3d &pos_local,
                        Vector3d &cos_local);
void transform_to_reference(Vector3d &pos_local,
                            Vector3d &cos_local,
                            Vector3d &origin,
                            Matrix3d &local_to_ref,
                            Vector3d &pos_ref,
                            Vector3d &cos_ref);
void compute_transform_matrices(Vector3d &euler,
                                Matrix3d &ref_to_local,
                                Matrix3d &local_to_ref);

#endif
