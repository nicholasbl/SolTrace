#include <gtest/gtest.h>

#include <common.hpp>

#include <composite_element.hpp>
#include <error_distributions.hpp>
#include <native_runner.hpp>
#include <native_runner_types.hpp>
#include <ray_source.hpp>
#include <sun.hpp>
#include <simulation_data.hpp>
#include <single_element.hpp>
#include <vector3d.hpp>

TEST(NativeRunnerTypes, TSun)
{

    SimulationData my_sim;
    auto sun = make_ray_source<Sun>();
    Vector3d spos(1.0, 2.0, 3.0);
    sun->set_position(spos);
    sun->set_shape(PILLBOX);
    my_sim.add_ray_source(sun);

    NativeRunner runner;
    runner.setup_sun(&my_sim);
    auto sys = runner.get_system();
    EXPECT_TRUE(is_identical(sys->Sun.Origin, sun->get_position()));
    EXPECT_EQ(sys->Sun.ShapeIndex, PILLBOX);
}

TEST(NativeRunnerTypes, TElement)
{
    // TODO: Implement a test here...

    // SimulationData my_sim;
    // // **** Setup Answers **** //
    // // Origin
    // Vector3d Origin1(1.0, 2.0, 3.0);
    // // Corresponding Euler angles in radians
    // const double a1 = 0.0;
    // const double b1 = asin(-1.0 / sqrt(3.0));
    // const double g1 = acos(1.0 / cos(b1) * 1.0 / sqrt(6.0)); // approximately 0.615
    // // Corresponding aim vector (local z-axis in reference coordinates)
    // Vector3d aim1(0.0, -1.0 / sqrt(3.0), sqrt(2.0 / 3.0));
    // vector_add(1.0, Origin1, 1.0, aim1);

    // // Z-Rotation is the last of the Euler angles but in degrees
    // const double zrot1 = g1 * 180.0 / M_PI;

    // // Origin
    // Vector3d Origin2(-3.0, 1.0, -5.0);
    // const double a2 = M_PI / 4.0;
    // const double b2 = M_PI / 6.0;
    // const double g2 = M_PI / 3.0;
    // // Corresponding aim vector (local z-axis in reference coordinates)
    // Vector3d aim2(sqrt(3.0 / 8.0), 0.5, sqrt(3.0 / 8.0));
    // vector_add(1.0, Origin2, 1.0, aim2);

    // // Z-Rotation is the last of the Euler angles but in degrees
    // const double zrot2 = 60.0;

    // // **** Setup Elements **** //
    // auto el = make_element<SingleElement>();
    // el->set_aperture(make_aperture<Circle>(2.0));
    // el->set_surface(make_surface<Flat>());
    // el->set_reference_frame_geometry(Origin1, aim1, zrot1);

    // auto st = make_stage(0);
    // st->set_reference_frame_geometry(Origin2, aim2, zrot2);
    // st->add_element(el);
}

TEST(NativeRunnerTypes, TStage)
{
    // TODO: Implement test
}

TEST(NativeRunner, SmokeTest)
{
    NativeRunner runner;
    SimulationData my_sim;

    my_sim.set_number_of_rays(10);
    my_sim.set_max_rays_traced(100);

    auto sun = make_ray_source<Sun>();
    sun->set_position(0.0, 0.0, 100.0);
    // sun->set_shape();
    my_sim.add_ray_source(sun);

    auto my_st = make_stage(0);
    const int NUM_ELEMENTS = 4;
    for (int k = 0; k < NUM_ELEMENTS; ++k)
    {
        element_ptr el = make_element<SingleElement>();
        el->set_aperture(make_aperture<Circle>(2.0));
        el->set_surface(make_surface<Flat>());
        my_st->add_element(el);
    }
    EXPECT_EQ(my_st->get_number_of_elements(), NUM_ELEMENTS);
    my_sim.add_stage(my_st);
    EXPECT_EQ(my_sim.get_number_of_elements(), NUM_ELEMENTS);
    
    RunnerStatus sts;
    sts = runner.initialize();
    EXPECT_EQ(sts, RunnerStatus::SUCCESS);
    sts = runner.setup_simulation(&my_sim);
    EXPECT_EQ(sts, RunnerStatus::SUCCESS);
    sts = runner.run_simulation();
    EXPECT_EQ(sts, RunnerStatus::SUCCESS);
}
