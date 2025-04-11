#include <gtest/gtest.h>

#include <vector3d.hpp>

#include "common.hpp"

TEST(LinearAlgebra, VectorBasics)
{
    Vector3d x(1.0, 1.0, 3.0);
    Vector3d y(-1.0, 2.0, -1.0);
    Vector3d z;

    vector_add(1.0, x, 1.0, y, z);
    EXPECT_NEAR(z[0], 0.0, 1e-12);
    EXPECT_NEAR(z[1], 3.0, 1e-12);
    EXPECT_NEAR(z[2], 2.0, 1e-12);

    vector_add(0.5, x, -2.0, y, z);
    EXPECT_NEAR(z[0], 2.5, 1e-12);
    EXPECT_NEAR(z[1], -3.5, 1e-12);
    EXPECT_NEAR(z[2], 3.5, 1e-12);

    vector_max(x, y, z);
    EXPECT_EQ(z[0], 1.0);
    EXPECT_EQ(z[1], 2.0);
    EXPECT_EQ(z[2], 3.0);

    vector_min(x, y, z);
    EXPECT_EQ(z[0], -1.0);
    EXPECT_EQ(z[1], 1.0);
    EXPECT_EQ(z[2], -1.0);

    double dp = dot_product(x, y);
    EXPECT_NEAR(dp, -2.0, 1e-12);

    double mag = vector_norm(x);
    EXPECT_NEAR(mag, sqrt(11.0), 1e-12);
    
    make_unit_vector(x);
    EXPECT_NEAR(x[0], 1.0 / mag, 1e-12);
    EXPECT_NEAR(x[1], 1.0 / mag, 1e-12);
    EXPECT_NEAR(x[2], 3.0 / mag, 1e-12);

    mag = vector_norm(y);
    EXPECT_NEAR(mag, sqrt(6.0), 1e-12);
    
    make_unit_vector(y);
    EXPECT_NEAR(y[0], -1.0 / mag, 1e-12);
    EXPECT_NEAR(y[1], 2.0 / mag, 1e-12);
    EXPECT_NEAR(y[2], -1.0 / mag, 1e-12);

}

TEST(LinearAlgebra, MatrixVectorProduct)
{
    Vector3d x(0.5, 1.0, -2.0);
    Vector3d y;

    Matrix3d A;
    A.set_value(0, 0, 1.0);
    A.set_value(1, 1, 1.0);
    A.set_value(2, 2, 1.0);

    matrix_vector_product(A, x, y);
    EXPECT_TRUE(is_identical(x, y));

    A.set_value(0, 1, 2.0);
    A.set_value(1, 2, 3.0);
    matrix_vector_product(A, x, y);
    EXPECT_NEAR(y[0], 2.5, 1e-12);
    EXPECT_NEAR(y[1], -5.0, 1e-12);
    EXPECT_NEAR(y[2], -2.0, 1e-12);

}

TEST(LinearAlgebra, CoordinateTransforms)
{
    // TODO: Implement tests for compute_transform_matrices, 
    // transform_to_local, and transform_to_reference functions
    Matrix3d A;
    Matrix3d B;

    EXPECT_TRUE(is_identical(A, B));    
}
