#include <gtest/gtest.h>

#include <composite_element.hpp>
#include <native_runner.hpp>
#include <native_runner_types.hpp>
#include <ray_source.hpp>
#include <sun.hpp>
#include <simulation_data.hpp>
#include <single_element.hpp>
#include <vector3d.hpp>

TEST(NativeRunnerTypes, TSun)
{
    auto sun = make_ray_source<Sun>();
    Vector3d spos(1.0, 2.0, 3.0);
    sun->set_position(spos);
    // TODO: Make test for below
    sun->set_shape();

    TSun my_sun;
    my_sun.set_values(sun);
    EXPECT_EQ(my_sun.Origin[0], spos[0]);
    EXPECT_EQ(my_sun.Origin[1], spos[1]);
    EXPECT_EQ(my_sun.Origin[2], spos[2]);
}

TEST(NativeRunnerTypes, TElement)
{
    // TODO: Implement test
}

TEST(NativeRunnerTypes, TStage)
{
    // TODO: Implement test
}

TEST(NativeRunner, SmokeTest)
{
    NativeRunner runner;
    SimulationData my_sim;
    my_sim.add_ray_source(make_ray_source<Sun>());
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
    // sts = runner.run_simulation();
    // EXPECT_EQ(sts, RunnerStatus::SUCCESS);
}
