#include <gtest/gtest.h>

#include <cmath>

#include <common.hpp>
#include <flat_calculator.hpp>
#include <surface.hpp>
#include <vector3d.hpp>

// NOTE: Equation for the plane is always z=0

TEST(FlatCalculator, Case1)
{
    // Case: mz = 0, parallel to plane -- returns no solution
    Vector3d zero;
    zero.zero();
    // Ray position
    Vector3d x0(2.0, 1.0, -3.0);
    // Ray direction
    Vector3d m(1.0, 1.0, 0.0);

    // Solution values
    double t;
    Vector3d xt;
    Vector3d mt;
    Vector3d gradf;

    FlatCalculator fcalc(nullptr);
    int sts = fcalc.intersect(x0.data, m.data,
                              xt.data, mt.data, gradf.data, &t);
    EXPECT_EQ(sts, 1);
    EXPECT_EQ(t, 0.0);
    EXPECT_TRUE(is_identical(xt, zero));
    EXPECT_TRUE(is_identical(mt, zero));
    EXPECT_TRUE(is_identical(gradf, zero));
}

TEST(FlatCalculator, Case2)
{
    // Case: t < 0.0 -- returns no solution
    Vector3d zero;
    zero.zero();
    // Ray position
    Vector3d x0(2.0, 1.0, -3.0);
    // Ray direction
    Vector3d m(1.0, 1.0, -2.0);

    // Solution values
    double t;
    Vector3d xt;
    Vector3d mt;
    Vector3d gradf;

    FlatCalculator fcalc(nullptr);
    int sts = fcalc.intersect(x0.data, m.data,
                              xt.data, mt.data, gradf.data, &t);
    EXPECT_EQ(sts, 1);
    EXPECT_EQ(t, 0.0);
    EXPECT_TRUE(is_identical(xt, zero));
    EXPECT_TRUE(is_identical(mt, zero));
    EXPECT_TRUE(is_identical(gradf, zero));
}

TEST(FlatCalculator, Case3)
{
    // Case: t < 0.0 -- returns no solution
    Vector3d zero;
    zero.zero();
    // Ray position
    Vector3d x0(2.0, 1.0, -3.0);
    // Ray direction
    Vector3d m(1.0, 1.0, 2.0);
    // Solution
    const double T = 1.5;
    const double TOL = 1e-12;

    // Solution values
    double t;
    Vector3d xt;
    Vector3d mt;
    Vector3d gradf;

    FlatCalculator fcalc(nullptr);
    int sts = fcalc.intersect(x0.data, m.data,
                              xt.data, mt.data, gradf.data, &t);
    EXPECT_EQ(sts, 0);
    EXPECT_NEAR(t, T, TOL);
    EXPECT_NEAR(xt[0], x0[0] + m[0] * T, TOL);
    EXPECT_NEAR(xt[1], x0[1] + m[1] * T, TOL);
    EXPECT_NEAR(xt[2], x0[2] + m[2] * T, TOL);
    EXPECT_NEAR(xt[2], 0.0, TOL);
    EXPECT_TRUE(is_identical(mt, m));
    EXPECT_NEAR(gradf[0], 0.0, TOL);
    EXPECT_NEAR(gradf[1], 0.0, TOL);
    EXPECT_NEAR(gradf[2], 1.0, TOL);
}

TEST(FlatCalculator, ZAperture)
{
    FlatCalculator fcalc(nullptr);
    EXPECT_EQ(fcalc.compute_z_aperture(nullptr), 0.0);
}
