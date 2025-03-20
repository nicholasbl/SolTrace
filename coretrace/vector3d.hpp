#ifndef SOLTRACE_VECTOR3D_H
#define SOLTRACE_VECTOR3D_H

#include <cassert>

#include "procs.h"

// Place holder for 3D vectors/points and 3x3 matrices

class Vector3d
{
public:
    Vector3d()
    {
        for (int i=0; i<3; ++i)
            this->data[i] = 0.0;
    }
    Vector3d(double data[3])
    {
        for(int i=0; i<3; ++i)
            this->data[i] = data[i];
    }
    Vector3d(double x, double y, double z)
    {
        this->data[0] = x;
        this->data[1] = y;
        this->data[2] = z;
    }
    ~Vector3d(){}

    inline double get_value(int i) const
    {
        assert(i >= 0 && i < 3);
        return this->data[i];
    }

    void set_value(int i, double val)
    {
        assert(i >= 0 && i < 3);
        this->data[i] = val;
        return;
    }

    void set_values(double x, double y, double z)
    {
        this->data[0] = x;
        this->data[1] = y;
        this->data[2] = z;
        return;
    }

    inline const double &operator[](int idx) const
    {
        return this->get_value(idx);
    }

    inline double & operator[](int idx)
    {   
        assert(idx >= 0 && idx < 3);
        return this->data[idx];
    }

    double data[3];

private:
};

class Matrix3d
{
public:
    Matrix3d()
    {
        for(int i=0; i<3; ++i)
            for (int j=0; j<3; ++j)
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

    // double data[9];
    double data[3][3];

private:
};

// Compute y = A*x placing the result in y
void matrix_vector_product(Matrix3d &A, Vector3d &x, Vector3d &y)
{
    MatrixVectorMult(A.data, x.data, y.data);
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

// Compute standard Euclidean dot product
double dot_product(Vector3d &x, Vector3d &y)
{
    return DOT(x.data, y.data);
}

double vector_norm(Vector3d &x)
{
    return sqrt(DOT(x.data, x.data));
}

void make_unit_vector(Vector3d &x)
{
    double mag = vector_norm(x);
    assert(mag > 0.0);
    for (int i = 0; i < 3; ++i)
    {
        x.data[0] /= mag;
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

#endif
