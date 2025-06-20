#include <gtest/gtest.h>

#include <cmath>

#include <common.hpp>
#include <parabola_calculator.hpp>
#include <surface.hpp>
#include <vector3d.hpp>

// NOTES: Equation for a parabola is z = (cx*x^2 + cy*y^2) / 2
// Computing the intersection point comes down to solving
// quadratic equation for a parameter t given by
//      at^2 + bt + c = 0
// The cases here are broken down by cases with regard to
// this equation. The case is noted at the top.  The coefficients
// a, b, and c can be determined by subtituting x(t) = x0 + mx*t,
// y(t) = y0 + my*t, and z(t) = z0 + mz*t into the given equation
// for a parabola. In the comments and naming, we use the
// following terms:
//    Delta = b^2 - 4ac  -- discrimanant of quadratic equation
//    t1 = (-b - sqrt(Delta)) / (2a) -- negative root
//    t2 = (-b + sqrt(Delta)) / (2a) -- positive root
// From these last two, we always have t1 < t2.

TEST(ParabolaCalculator, Case1)
{
    // Case: a == 0, t <= 0 -- returns no solution
    Vector3d zero;
    zero.zero();
    // Ray location
    Vector3d x0(0.0, 0.0, 1.0);
    // Ray direction
    Vector3d m(0.0, 0.0, 1.0);

    // Solution values
    double t;
    Vector3d xt;
    Vector3d mt;
    Vector3d gradf;

    auto parabola = make_surface<Parabola>(0.5, 0.25);
    ParabolaCalculator pcalc(parabola);
    int sts = pcalc.intersect(x0.data, m.data,
                              xt.data, mt.data, gradf.data, &t);
    EXPECT_EQ(sts, 1);
    EXPECT_EQ(t, 0.0);
    EXPECT_TRUE(is_identical(xt, zero));
    EXPECT_TRUE(is_identical(mt, zero));
    EXPECT_TRUE(is_identical(gradf, zero));
}

TEST(ParabolaCalculator, Case2)
{
    // Case: a == 0, t > 0 -- returns t
    // NOTE: Here the quadratic equation reduces to a linear equation
    // Ray location
    Vector3d x0(0.0, 0.0, -1.0);
    // Ray direction
    Vector3d m(0.0, 0.0, 1.0);
    // Intersection point
    const double T = 1.0;
    const double TOL = 1e-12;
    // Parabola constants
    const double cx = 1.0;
    const double cy = 2.0;

    // Solution values
    double t;
    Vector3d xt;
    Vector3d mt;
    Vector3d gradf;

    auto parabola = make_surface<Parabola>(0.5, 0.25);
    ParabolaCalculator pcalc(parabola);
    int sts = pcalc.intersect(x0.data, m.data,
                              xt.data, mt.data, gradf.data, &t);
    EXPECT_EQ(sts, 0);
    EXPECT_NEAR(t, T, TOL);
    EXPECT_NEAR(xt[0], 0.0, TOL);
    EXPECT_NEAR(xt[1], 0.0, TOL);
    EXPECT_NEAR(xt[2], 0.0, TOL);
    EXPECT_TRUE(is_identical(mt, m));
    EXPECT_NEAR(gradf[0], 0.0, TOL);
    EXPECT_NEAR(gradf[1], 0.0, TOL);
    EXPECT_NEAR(gradf[2], 1.0, TOL);
    EXPECT_NEAR(xt[2] - 0.5 * (cx * xt[0] * xt[0] + cy * xt[1] * xt[1]), 0.0, TOL);
}

TEST(ParabolaCalculator, Case3)
{
    // Case: a != 0, Delta = b^2 - 4ac < 0 -- returns no solution
    Vector3d zero;
    zero.zero();
    // Ray location
    Vector3d x0(-2.0, 1.0, 0.0);
    // Ray direction
    Vector3d m(1.0, 1.0, 1.0);

    // Solution values
    double t;
    Vector3d xt;
    Vector3d mt;
    Vector3d gradf;

    auto parabola = make_surface<Parabola>(0.5, 0.25);
    ParabolaCalculator pcalc(parabola);
    int sts = pcalc.intersect(x0.data, m.data,
                              xt.data, mt.data, gradf.data, &t);
    EXPECT_EQ(sts, 1);
    EXPECT_EQ(t, 0.0);
    EXPECT_TRUE(is_identical(xt, zero));
    EXPECT_TRUE(is_identical(mt, zero));
    EXPECT_TRUE(is_identical(gradf, zero));
}

TEST(ParabolaCalculator, Case4)
{
    // Case: a != 0, Delta >= 0, t1 > 0 -- returns t1
    // Ray location
    Vector3d x0(-2.0, 1.0, 0.0);
    // Ray direction
    Vector3d m(2.0, 0.5, 3.0);
    // Intersection point
    const double T = 2.0 / 3.0;
    const double TOL = 1e-12;
    // Parabola constants
    const double cx = 1.0;
    const double cy = 2.0;

    // Solution values
    double t;
    Vector3d xt;
    Vector3d mt;
    Vector3d gradf;

    auto parabola = make_surface<Parabola>(0.5 / cx, 0.5 / cy);
    ParabolaCalculator pcalc(parabola);
    int sts = pcalc.intersect(x0.data, m.data,
                              xt.data, mt.data, gradf.data, &t);
    EXPECT_EQ(sts, 0);
    EXPECT_NEAR(t, T, TOL);
    EXPECT_NEAR(xt[0], m[0] * T + x0[0], TOL);
    EXPECT_NEAR(xt[1], m[1] * T + x0[1], TOL);
    EXPECT_NEAR(xt[2], m[2] * T + x0[2], TOL);
    EXPECT_TRUE(is_identical(mt, m));
    EXPECT_NEAR(gradf[0], -cx * (m[0] * T + x0[0]), TOL);
    EXPECT_NEAR(gradf[1], -cy * (m[1] * T + x0[1]), TOL);
    EXPECT_NEAR(gradf[2], 1.0, TOL);
    EXPECT_NEAR(xt[2] - 0.5 * (cx * xt[0] * xt[0] + cy * xt[1] * xt[1]), 0.0, TOL);
}

TEST(ParabolaCalculator, Case5)
{
    // Case: a != 0, Delta >= 0, t1 < 0, t2 > 0 -- returns t2
    // Ray location
    Vector3d x0(1.0, -1.0, 3.0);
    // Ray direction
    Vector3d m(1.0, 0.0, 2.0);
    // Intersection point
    const double T = 3.0;
    const double TOL = 1e-12;
    // Parabola constants
    const double cx = 1.0;
    const double cy = 2.0;

    // Solution values
    double t;
    Vector3d xt;
    Vector3d mt;
    Vector3d gradf;

    auto parabola = make_surface<Parabola>(0.5 / cx, 0.5 / cy);
    ParabolaCalculator pcalc(parabola);
    int sts = pcalc.intersect(x0.data, m.data,
                              xt.data, mt.data, gradf.data, &t);
    EXPECT_EQ(sts, 0);
    EXPECT_NEAR(t, T, TOL);
    EXPECT_NEAR(xt[0], m[0] * T + x0[0], TOL);
    EXPECT_NEAR(xt[1], m[1] * T + x0[1], TOL);
    EXPECT_NEAR(xt[2], m[2] * T + x0[2], TOL);
    EXPECT_TRUE(is_identical(mt, m));
    EXPECT_NEAR(gradf[0], -cx * (m[0] * T + x0[0]), TOL);
    EXPECT_NEAR(gradf[1], -cy * (m[1] * T + x0[1]), TOL);
    EXPECT_NEAR(gradf[2], 1.0, TOL);
    EXPECT_NEAR(xt[2] - 0.5 * (cx * xt[0] * xt[0] + cy * xt[1] * xt[1]), 0.0, TOL);
}

TEST(ParabolaCalculator, Case6)
{
    // Case: a != 0, Delta >= 0, t1 < 0, t2 < 0 -- returns no solution
    Vector3d zero;
    zero.zero();
    // Ray location
    Vector3d x0(3.0, 1.0, 2.0);
    // Ray direction
    Vector3d m(1.0, -1.0, -4.0);
    // Intersection point
    const double T = 1.0 + sqrt(8.0);
    const double TOL = 1e-12;
    // Parabola constants
    const double cx = 1.0;
    const double cy = 2.0;

    // Solution values
    double t;
    Vector3d xt;
    Vector3d mt;
    Vector3d gradf;

    auto parabola = make_surface<Parabola>(0.5, 0.25);
    ParabolaCalculator pcalc(parabola);
    int sts = pcalc.intersect(x0.data, m.data,
                              xt.data, mt.data, gradf.data, &t);
    EXPECT_EQ(sts, 1);
    EXPECT_EQ(t, 0.0);
    EXPECT_TRUE(is_identical(xt, zero));
    EXPECT_TRUE(is_identical(mt, zero));
    EXPECT_TRUE(is_identical(gradf, zero));
}
