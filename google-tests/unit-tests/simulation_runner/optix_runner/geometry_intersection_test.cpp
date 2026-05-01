#include <gtest/gtest.h>

#include <optix_runner.hpp>
#include <simulation_data_export.hpp>
#include <simulation_result_export.hpp>
#include <simulation_runner.hpp>

using SolTrace::Runner::RunnerStatus;

const double Z_ELEM = 50.0;
const double TOL = 1e-6;
const uint_fast64_t NRAYS = 10000;

void set_default_sd(SimulationData &sd,
                    surface_ptr surf,
                    aperture_ptr ap)
{
    sd.clear();

    // Sun
    auto sun = make_ray_source<Sun>();
    sun->set_position(0, 0, 100);
    sd.add_ray_source(sun);

    // Make reflective flat el
    element_ptr el = make_element<SingleElement>();
    el->set_origin(0, 0, Z_ELEM);
    el->set_aim_vector(0, 0, 100); // Face up towards sun
    el->set_surface(surf);
    el->set_aperture(ap);

    OpticalProperties el_optics;
    el_optics.set_ideal_absorption();
    el->set_front_optical_properties(el_optics);
    el->set_back_optical_properties(el_optics);
    el->set_name("el");

    // Add element to stage
    sd.add_element(el);

    // Set parameters
    SimulationParameters &params = sd.get_simulation_parameters();
    params.number_of_rays = NRAYS;
    params.max_number_of_rays = params.number_of_rays * 100;
    params.include_optical_errors = false;
    params.include_sun_shape_errors = false;
    params.seed = 123;
}

TEST(OptixRunner, FlatRectangle)
{
    const double XL = 10.0;
    const double YL = 5.0;
    auto surf = make_surface<Flat>();
    auto aper = make_aperture<Rectangle>(XL, YL);

    SimulationData sd;
    set_default_sd(sd, surf, aper);
    SimulationResult result;

    OptixRunner runner;
    RunnerStatus sts = runner.initialize();
    ASSERT_EQ(sts, RunnerStatus::SUCCESS);
    sts = runner.setup_simulation(&sd);
    ASSERT_EQ(sts, RunnerStatus::SUCCESS);
    sts = runner.run_simulation();
    ASSERT_EQ(sts, RunnerStatus::SUCCESS);
    sts = runner.report_simulation(&result, 0);
    ASSERT_EQ(sts, RunnerStatus::SUCCESS);

    ASSERT_EQ(result.get_number_of_records(),
              sd.get_simulation_parameters().number_of_rays);
    for (int i = 0; i < (int)result.get_number_of_records(); ++i)
    {
        auto rr = result[i];
        ASSERT_GE(rr->get_number_of_interactions(), 2);
        glm::dvec3 p0, p1;
        rr->get_position(0, p0);
        rr->get_position(1, p1);
        EXPECT_NEAR(p0[0], p1[0], TOL) << "ray " << i;
        EXPECT_NEAR(p0[1], p1[1], TOL) << "ray " << i;
        EXPECT_NEAR(p1[2], Z_ELEM, TOL * Z_ELEM) << "ray " << i;
    }
}

TEST(OptixRunner, FlatEquilateralTriangle)
{
    const double d = 4.0;
    auto surf = make_surface<Flat>();
    auto aper = make_aperture<EquilateralTriangle>(d);

    SimulationData sd;
    set_default_sd(sd, surf, aper);
    SimulationResult result;

    OptixRunner runner;
    RunnerStatus sts = runner.initialize();
    ASSERT_EQ(sts, RunnerStatus::SUCCESS);
    sts = runner.setup_simulation(&sd);
    ASSERT_EQ(sts, RunnerStatus::SUCCESS);
    sts = runner.run_simulation();
    ASSERT_EQ(sts, RunnerStatus::SUCCESS);
    sts = runner.report_simulation(&result, 0);
    ASSERT_EQ(sts, RunnerStatus::SUCCESS);

    ASSERT_EQ(result.get_number_of_records(),
              sd.get_simulation_parameters().number_of_rays);
    for (int i = 0; i < (int)result.get_number_of_records(); ++i)
    {
        auto rr = result[i];
        ASSERT_GE(rr->get_number_of_interactions(), 2);
        glm::dvec3 p0, p1;
        rr->get_position(0, p0);
        rr->get_position(1, p1);
        EXPECT_NEAR(p0[0], p1[0], TOL) << "ray " << i;
        EXPECT_NEAR(p0[1], p1[1], TOL) << "ray " << i;
        EXPECT_NEAR(p1[2], Z_ELEM, TOL * Z_ELEM) << "ray " << i;
    }
}

TEST(OptixRunner, FlatTriangle)
{
    const double x1 = 0.0, x2 = 1.0, x3 = 2.0 * x2;
    const double y1 = 0.0, y2 = 2.0, y3 = y1;
    auto surf = make_surface<Flat>();
    auto aper = make_aperture<IrregularTriangle>(x1, y1, x2, y2, x3, y3);

    SimulationData sd;
    set_default_sd(sd, surf, aper);
    SimulationResult result;

    OptixRunner runner;
    RunnerStatus sts = runner.initialize();
    ASSERT_EQ(sts, RunnerStatus::SUCCESS);
    sts = runner.setup_simulation(&sd);
    ASSERT_EQ(sts, RunnerStatus::SUCCESS);
    sts = runner.run_simulation();
    ASSERT_EQ(sts, RunnerStatus::SUCCESS);
    sts = runner.report_simulation(&result, 0);
    ASSERT_EQ(sts, RunnerStatus::SUCCESS);

    ASSERT_EQ(result.get_number_of_records(),
              sd.get_simulation_parameters().number_of_rays);
    for (int i = 0; i < (int)result.get_number_of_records(); ++i)
    {
        auto rr = result[i];
        ASSERT_GE(rr->get_number_of_interactions(), 2);
        glm::dvec3 p0, p1;
        rr->get_position(0, p0);
        rr->get_position(1, p1);
        EXPECT_NEAR(p0[0], p1[0], TOL) << "ray " << i;
        EXPECT_NEAR(p0[1], p1[1], TOL) << "ray " << i;
        EXPECT_NEAR(p1[2], Z_ELEM, TOL * Z_ELEM) << "ray " << i;
    }
}

TEST(OptixRunner, FlatQuadrilateral)
{
    // Parallelogram
    const double x1 = 0.0, x2 = 3.0, x3 = (x2 - x1) + 1.0, x4 = x3 - x2 + x1;
    const double y1 = 0.0, y2 = y1, y3 = 2.0, y4 = y3;
    auto surf = make_surface<Flat>();
    auto aper = make_aperture<IrregularQuadrilateral>(
        x1, y1, x2, y2, x3, y3, x4, y4);

    SimulationData sd;
    set_default_sd(sd, surf, aper);
    SimulationResult result;

    OptixRunner runner;
    RunnerStatus sts = runner.initialize();
    ASSERT_EQ(sts, RunnerStatus::SUCCESS);
    sts = runner.setup_simulation(&sd);
    ASSERT_EQ(sts, RunnerStatus::SUCCESS);
    sts = runner.run_simulation();
    ASSERT_EQ(sts, RunnerStatus::SUCCESS);
    sts = runner.report_simulation(&result, 0);
    ASSERT_EQ(sts, RunnerStatus::SUCCESS);

    ASSERT_EQ(result.get_number_of_records(),
              sd.get_simulation_parameters().number_of_rays);
    for (int i = 0; i < (int)result.get_number_of_records(); ++i)
    {
        auto rr = result[i];
        ASSERT_GE(rr->get_number_of_interactions(), 2);
        glm::dvec3 p0, p1;
        rr->get_position(0, p0);
        rr->get_position(1, p1);
        EXPECT_NEAR(p0[0], p1[0], TOL) << "ray " << i;
        EXPECT_NEAR(p0[1], p1[1], TOL) << "ray " << i;
        EXPECT_NEAR(p1[2], Z_ELEM, TOL * Z_ELEM) << "ray " << i;
    }
}

TEST(OptixRunner, ParabolaRectangle)
{
    constexpr double CX = 0.5;
    constexpr double CY = 1.0;
    constexpr double FX = 0.5 / CX;
    constexpr double FY = 0.5 / CY;
    const double XL = 10.0, YL = 5.0;
    auto surf = make_surface<Parabola>(FX, FY);
    auto aper = make_aperture<Rectangle>(XL, YL);

    SimulationData sd;
    set_default_sd(sd, surf, aper);
    SimulationResult result;

    OptixRunner runner;
    RunnerStatus sts = runner.initialize();
    ASSERT_EQ(sts, RunnerStatus::SUCCESS);
    sts = runner.setup_simulation(&sd);
    ASSERT_EQ(sts, RunnerStatus::SUCCESS);
    sts = runner.run_simulation();
    ASSERT_EQ(sts, RunnerStatus::SUCCESS);
    sts = runner.report_simulation(&result, 0);
    ASSERT_EQ(sts, RunnerStatus::SUCCESS);

    ASSERT_EQ(result.get_number_of_records(),
              sd.get_simulation_parameters().number_of_rays);
    for (int i = 0; i < (int)result.get_number_of_records(); ++i)
    {
        auto rr = result[i];
        ASSERT_GE(rr->get_number_of_interactions(), 2);
        glm::dvec3 p0, p1;
        rr->get_position(0, p0);
        rr->get_position(1, p1);
        EXPECT_NEAR(p0[0], p1[0], TOL) << "ray " << i;
        EXPECT_NEAR(p0[1], p1[1], TOL) << "ray " << i;
        const double z1 = Z_ELEM + 0.5 * CX * p1[0] * p1[0] + 0.5 * CY * p1[1] * p1[1];
        EXPECT_NEAR(p1[2], z1, TOL * Z_ELEM) << "ray " << i;
    }
}

TEST(OptixRunner, Cylinder)
{
    const double R = 5.0;
    const double YL = 3.0; // Total cylinder length
    auto surf = make_surface<Cylinder>(R);
    auto aper = make_aperture<Rectangle>(2 * R, YL);

    SimulationData sd;
    set_default_sd(sd, surf, aper);
    SimulationResult result;

    OptixRunner runner;
    RunnerStatus sts = runner.initialize();
    ASSERT_EQ(sts, RunnerStatus::SUCCESS);
    sts = runner.setup_simulation(&sd);
    ASSERT_EQ(sts, RunnerStatus::SUCCESS);
    sts = runner.run_simulation();
    ASSERT_EQ(sts, RunnerStatus::SUCCESS);
    sts = runner.report_simulation(&result, 0);
    ASSERT_EQ(sts, RunnerStatus::SUCCESS);

    ASSERT_EQ(result.get_number_of_records(),
              sd.get_simulation_parameters().number_of_rays);
    for (int i = 0; i < (int)result.get_number_of_records(); ++i)
    {
        auto rr = result[i];
        ASSERT_GE(rr->get_number_of_interactions(), 2);
        glm::dvec3 p0, p1;
        rr->get_position(0, p0);
        rr->get_position(1, p1);
        EXPECT_NEAR(p0[0], p1[0], TOL) << "ray " << i;
        EXPECT_NEAR(p0[1], p1[1], TOL) << "ray " << i;
        const double z1 = Z_ELEM + sqrt(R * R - p1[0] * p1[0]);
        EXPECT_NEAR(p1[2], z1, TOL * Z_ELEM) << "ray " << i;
    }
}
