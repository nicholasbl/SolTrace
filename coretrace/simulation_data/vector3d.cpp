#include "glm::dvec3.hpp"

// #include <cassert>

#include <algorithm>
#include <cmath>

#if 0

#include "matvec.hpp"

namespace SolTrace::Data
{

    // inline void vector_copy(double data[3], const glm::dvec3 &x)
    // {
    //     CopyVec3(data, x.data);
    //     return;
    // }
    // inline void vector_copy(std::vector<double> &dest, const glm::dvec3 &x)
    // {
    //     CopyVec3(dest, x.data);
    //     return;
    // }

    glm::dvec3::glm::dvec3()
    {
        this->zero();
        return;
    }

    glm::dvec3::glm::dvec3(const double data[3])
    {
        for (int i = 0; i < 3; ++i)
            this->data[i] = data[i];
        return;
    }
    glm::dvec3::glm::dvec3(double x, double y, double z)
    {
        this->data[0] = x;
        this->data[1] = y;
        this->data[2] = z;
        return;
    }
    glm::dvec3::~glm::dvec3()
    {
        return;
    }

    void glm::dvec3::zero()
    {
        for (int i = 0; i < 3; ++i)
            this->data[i] = 0.0;
        return;
    }

    void glm::dvec3::set_values(double x, double y, double z)
    {
        this->data[0] = x;
        this->data[1] = y;
        this->data[2] = z;
        return;
    }

    void glm::dvec3::scalar_mult(double alpha)
    {
        for (int i = 0; i < 3; ++i)
            this->data[i] *= alpha;
        return;
    }

    void glm::dvec3::make_unit()
    {
        make_unit_vector(*this);
        return;
    }

    double glm::dvec3::norm() const
    {
        return vector_norm(*this);
    }

    const double &glm::dvec3::operator[](int idx) const
    {
        assert(idx >= 0 && idx < 3);
        return this->data[idx];
    }

    double &glm::dvec3::operator[](int idx)
    {
        assert(idx >= 0 && idx < 3);
        return this->data[idx];
    }

    std::ostream &operator<<(std::ostream &os, const glm::dvec3 &x)
    {
        os << "[" << x.data[0] << ", "
           << x.data[1] << ", "
           << x.data[2] << "]";
        return os;
    }

    Matrix3d::Matrix3d()
    {
        // for (int i = 0; i < 3; ++i)
        //     for (int j = 0; j < 3; ++j)
        //         this->data[i][j] = 0.0;
        this->zero();
    }
    Matrix3d::~Matrix3d() {}

    void Matrix3d::set_value(int i, int j, double val)
    {
        assert(i >= 0 && i < 3);
        assert(j >= 0 && j < 3);
        this->data[i][j] = val;
    }

    double Matrix3d::get_value(int i, int j) const
    {
        assert(i >= 0 && i < 3);
        assert(j >= 0 && j < 3);
        return this->data[i][j];
    }

    void Matrix3d::zero()
    {
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                data[i][j] = 0.0;
            }
        }
    }

    void Matrix3d::identity()
    {
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                data[i][j] = (i == j ? 1.0 : 0.0);
            }
        }
    }

    std::ostream &operator<<(std::ostream &os, const Matrix3d &A)
    {
        // os << "[" << x.data[0] << ", "
        //    << x.data[1] << ", "
        //    << x.data[2] << "]";
        os << "[";
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                os << A.data[i][j];
                if (j < 2)
                {
                    os << ", ";
                }
            }
            if (i < 2)
            {
                os << "; ";
            }
        }
        os << "]";
        return os;
    }

    // Compute y = A*x placing the result in y
    void matrix_vector_product(const Matrix3d &A, const glm::dvec3 &x, glm::dvec3 &y)
    {
        MatrixVectorMult(A.data, x.data, y.data);
        return;
    }

    void matrix_matrix_product(const Matrix3d &A, const Matrix3d &B, Matrix3d &C)
    {
        MatrixMatrixMult(A.data, B.data, C.data);
        return;
    }

    void vector_add(double a, const glm::dvec3 &x,
                    double b, const glm::dvec3 &y,
                    glm::dvec3 &z)
    {
        for (int i = 0; i < 3; ++i)
        {
            z[i] = a * x[i] + b * y[i];
        }
        return;
    }

    void vector_add(double a, const glm::dvec3 &x,
                    double b, glm::dvec3 &y)
    {
        for (int i = 0; i < 3; ++i)
        {
            y[i] = a * x[i] + b * y[i];
        }
        return;
    }

    void vector_max(const glm::dvec3 &x, const glm::dvec3 &y, glm::dvec3 &max)
    {
        for (int i = 0; i < 3; ++i)
        {
            max[i] = fmax(x[i], y[i]);
        }
        return;
    }

    void vector_min(const glm::dvec3 &x, const glm::dvec3 &y, glm::dvec3 &min)
    {
        for (int i = 0; i < 3; ++i)
        {
            min[i] = fmin(x[i], y[i]);
        }
        return;
    }

    // Compute standard Euclidean dot product
    double dot_product(const glm::dvec3 &x, const glm::dvec3 &y)
    {
        return DOT(x.data, y.data);
    }

    void cross_product(const glm::dvec3 &u, const glm::dvec3 &v, glm::dvec3 &w)
    {
        double w0 = u[1] * v[2] - u[2] * v[1];
        double w1 = u[2] * v[0] - u[0] * v[2];
        double w2 = u[0] * v[1] - u[1] * v[0];
        w.set_values(w0, w1, w2);
        return;
    }

    double error(const glm::dvec3 &u, const glm::dvec3 &v)
    {
        double err = 0.0;
        double dx;
        for (int i = 0; i < 3; ++i)
        {
            dx = u[i] - v[i];
            err += dx * dx;
        }
        return sqrt(err);
    }

    double error_inf(const glm::dvec3 &u, const glm::dvec3 &v)
    {
        double err = 0.0;
        for (int i = 0; i < 3; ++i)
        {
            err = std::max(err, fabs(u[i] - v[i]));
        }
        return err;
    }

    double vector_norm(const glm::dvec3 &x)
    {
        return sqrt(DOT(x.data, x.data));
    }

    void make_unit_vector(glm::dvec3 &x)
    {
        double mag = vector_norm(x);
        assert(mag > 0.0);
        x.scalar_mult(1.0 / mag);
        // for (int i = 0; i < 3; ++i)
        // {
        //     x.data[i] /= mag;
        // }
        return;
    }

    // void transform_to_local(glm::dvec3 &pos_ref,
    //                         glm::dvec3 &cos_ref,
    //                         glm::dvec3 &origin,
    //                         Matrix3d &ref_to_local,
    //                         glm::dvec3 &pos_local,
    //                         glm::dvec3 &cos_local)
    // {
    //     TransformToLocal(pos_ref.data, cos_ref.data,
    //                      origin.data, ref_to_local.data,
    //                      pos_local.data, cos_local.data);
    //     return;
    // }

    // void transform_to_reference(glm::dvec3 &pos_local,
    //                             glm::dvec3 &cos_local,
    //                             glm::dvec3 &origin,
    //                             Matrix3d &local_to_ref,
    //                             glm::dvec3 &pos_ref,
    //                             glm::dvec3 &cos_ref)
    // {
    //     TransformToReference(pos_local.data, cos_local.data,
    //                          origin.data, local_to_ref.data,
    //                          pos_ref.data, cos_ref.data);
    //     return;
    // }

    void compute_transform_matrices(glm::dvec3 &euler,
                                    Matrix3d &ref_to_local,
                                    Matrix3d &local_to_ref)
    {
        CalculateTransformMatrices(euler.data,
                                   ref_to_local.data,
                                   local_to_ref.data);
        return;
    }

} // namespace SolTrace::Data

#endif
