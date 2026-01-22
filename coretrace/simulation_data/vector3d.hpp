/**
 * @file glm::dvec3.hpp
 * @brief 3D vector and matrix classes
 *
 * Provides glm::dvec3 and Matrix3d classes for 3D geometric operations,
 * coordinate transformations, and linear algebra computations.
 * These classes form the mathematical foundation for ray tracing
 * calculations and geometric transformations in SolTrace.
 *
 * @defgroup math Mathematical Operations
 * @{
 */

#ifndef SOLTRACE_glm_vec3
#define SOLTRACE_glm_vec3

#include <cassert>
#include <vector>
#if 0
#include "matvec.hpp"

namespace SolTrace::Data {

class glm::dvec3
{
public:
    /**
     * @brief Default constructor - initializes to zero vector
     */
    glm::dvec3();

    /**
     * @brief Constructor from array
     * @param data Array of 3 doubles [x, y, z]
     */
    glm::dvec3(const double data[3]);

    /**
     * @brief Constructor from components
     * @param x X component
     * @param y Y component
     * @param z Z component
     */
    glm::dvec3(double x, double y, double z);
    ~glm::dvec3();

    /**
     * @brief Set all components to zero
     */
    void zero();

    /**
     * @brief Set vector components
     * @param x X component
     * @param y Y component
     * @param z Z component
     */
    void set_values(double x, double y, double z);

    /**
     * @brief Multiply vector by scalar
     * @param alpha Scalar multiplier
     */
    void scalar_mult(double alpha);

    /**
     * @brief Make the vector a unit vector
     */
    void make_unit();

    /**
     * @brief Compute Euclidean vector norm
     * @return Euclidean norm
     */
    double norm() const;

    /**
     * @brief Const access operator
     * @param idx Component index (0=x, 1=y, 2=z)
     * @return Const reference to component
     */
    const double &operator[](int idx) const;

    /**
     * @brief Access operator
     * @param idx Component index (0=x, 1=y, 2=z)
     * @return Reference to component
     */
    double &operator[](int idx);

    /**
     * @brief Stream output operator
     * @param os Output stream
     * @param x Vector to output
     * @return Reference to output stream
     */
    friend std::ostream &operator<<(std::ostream &os, const glm::dvec3 &x);

    double data[3];

private:
};

class Matrix3d
{
public:
    /**
     * @brief Default constructor - initializes to zero matrix
     */
    Matrix3d();
    ~Matrix3d();

    /**
     * @brief Set matrix element value
     * @param i Row index (0-2)
     * @param j Column index (0-2)
     * @param val Value to set
     */
    void set_value(int i, int j, double val);

    /**
     * @brief Get matrix element value
     * @param i Row index (0-2)
     * @param j Column index (0-2)
     * @return Matrix element value
     */
    double get_value(int i, int j) const;

    /**
     * @brief Set all matrix elements to zero
     */
    void zero();

    /**
     * @brief Set matrix to identity (diagonal = 1, off-diagonal = 0)
     */
    void identity();

    // double data[9];
    double data[3][3];

    friend std::ostream &operator<<(std::ostream &os, const Matrix3d &A);

private:
};

inline void vector_copy(double data[3], const glm::dvec3 &x)
{
    CopyVec3(data, x.data);
    return;
}
inline void vector_copy(std::vector<double> &dest, const glm::dvec3 &x)
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
void matrix_vector_product(const Matrix3d &A, const glm::dvec3 &x, glm::dvec3 &y);
// Compute C = A * B placing result in C
void matrix_matrix_product(const Matrix3d &A, const Matrix3d &B, Matrix3d &C);

// Compute z = a*x + b*y (result stored in z)
void vector_add(double a, const glm::dvec3 &x,
                double b, const glm::dvec3 &y,
                glm::dvec3 &z);
/**
 * @brief Compute linear combination y = a*x + b*y (result stored in y)
 * @param a Scalar multiplier for vector x
 * @param x First vector
 * @param b Scalar multiplier for vector y
 * @param y Second vector (modified in-place with result)
 */
void vector_add(double a, const glm::dvec3 &x,
                double b, glm::dvec3 &y);

/**
 * @brief Compute element-wise maximum of two vectors
 * @param x First vector
 * @param y Second vector
 * @param max Output vector containing element-wise maximum
 */
void vector_max(const glm::dvec3 &x, const glm::dvec3 &y, glm::dvec3 &max);

/**
 * @brief Compute element-wise minimum of two vectors
 * @param x First vector
 * @param y Second vector
 * @param min Output vector containing element-wise minimum
 */
void vector_min(const glm::dvec3 &x, const glm::dvec3 &y, glm::dvec3 &min);

/**
 * @brief Compute standard Euclidean dot product
 * @param x First vector
 * @param y Second vector
 * @return Dot product x·y
 */
double dot_product(const glm::dvec3 &x, const glm::dvec3 &y);

/**
 * @brief Compute standard Euclidean cross product
 * @param u First vector
 * @param v Second vector
 * @param w Result vector
 */
void cross_product(const glm::dvec3 &u, const glm::dvec3 &v, glm::dvec3 &w);

double error(const glm::dvec3 &u, const glm::dvec3 &v);
double error_inf(const glm::dvec3 &u, const glm::dvec3 &v);

/**
 * @brief Compute Euclidean norm (length) of vector
 * @param x Input vector
 * @return ||x||₂ (L2 norm)
 */
double vector_norm(const glm::dvec3 &x);

/**
 * @brief Normalize vector to unit length (modifies vector in-place)
 * @param x Vector to normalize
 */
void make_unit_vector(glm::dvec3 &x);
// void transform_to_local(glm::dvec3 &pos_ref,
//                         glm::dvec3 &cos_ref,
//                         glm::dvec3 &origin,
//                         Matrix3d &ref_to_local,
//                         glm::dvec3 &pos_local,
//                         glm::dvec3 &cos_local);
// void transform_to_reference(glm::dvec3 &pos_local,
//                             glm::dvec3 &cos_local,
//                             glm::dvec3 &origin,
//                             Matrix3d &local_to_ref,
//                             glm::dvec3 &pos_ref,
//                             glm::dvec3 &cos_ref);

/**
 * @brief Compute transformation matrices from Euler angles
 * @param euler Euler angles vector [rotation_x, rotation_y, rotation_z] in radians
 * @param ref_to_local Output matrix for reference-to-local transformation
 * @param local_to_ref Output matrix for local-to-reference transformation
 */
void compute_transform_matrices(glm::dvec3 &euler,
                                Matrix3d &ref_to_local,
                                Matrix3d &local_to_ref);

} // namespace SolTrace::Data

/**
 * @}
 */
#endif

#endif
