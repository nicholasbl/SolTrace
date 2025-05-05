#include <gtest/gtest.h>

#include <composite_element.hpp>
#include <single_element.hpp>
#include <sun.hpp>
#include <simulation_data.hpp>

TEST(SimulationData, ElementInterface)
{
    // auto my_reflector = std::make_shared<PlaneReflector>();
    SimulationData my_sim;

    auto my_reflector = make_element<SingleElement>();
    auto id1 = my_sim.add_element(my_reflector);
    auto my_comp = make_element<CompositeElement>();
    auto id2 = my_sim.add_element(my_comp);

    EXPECT_EQ(my_sim.get_number_of_elements(), 2);
    EXPECT_NE(id1, id2);
    EXPECT_EQ(my_sim.get_element(id1), my_reflector);
    EXPECT_EQ(my_sim.get_element(id2), my_comp);

    element_id nonexistant = id2 + 1000;
    EXPECT_EQ(my_sim.get_element(nonexistant), nullptr);
    EXPECT_EQ(my_sim.remove_element(nonexistant), 0);
    EXPECT_EQ(my_sim.get_number_of_elements(), 2);

    EXPECT_EQ(my_sim.remove_element(id1), 1);
    EXPECT_EQ(my_sim.get_number_of_elements(), 1);
    EXPECT_EQ(my_sim.get_element(id1), nullptr);

    auto my_el = make_element<SingleElement>();
    EXPECT_FALSE(my_sim.replace_element(nonexistant, my_el));
    EXPECT_TRUE(my_sim.replace_element(id2, my_el));
    EXPECT_EQ(my_sim.get_element(id2), my_el);

    my_sim.add_element(my_reflector);
    my_sim.add_element(my_comp);
    EXPECT_EQ(my_sim.get_number_of_elements(), 3);

    // These are pass through calls and tested fully in the container_test.cpp
    // Here just call them to make sure the calls work.
    auto iter = my_sim.get_iterator();
    EXPECT_FALSE(my_sim.is_at_end(iter));
    auto citer = my_sim.get_const_iterator();
    EXPECT_FALSE(my_sim.is_at_end(citer));
}

TEST(SimulationData, RaySourceInterface)
{
    SimulationData my_sim;

    auto src1 = make_ray_source<Sun>();
    auto id1 = my_sim.add_ray_source(src1);
    auto src2 = make_ray_source<Sun>();
    auto id2 = my_sim.add_ray_source(src2);
    auto src3 = make_ray_source<Sun>();

    EXPECT_EQ(my_sim.get_ray_source(id1), src1);
    EXPECT_EQ(my_sim.get_ray_source(id2), src2);
    EXPECT_NE(id1, id2);

    ray_source_id nonexistant = id2 + 1000;
    EXPECT_EQ(my_sim.get_ray_source(nonexistant), nullptr);
    EXPECT_EQ(my_sim.remove_ray_source(nonexistant), 0);
    EXPECT_FALSE(my_sim.replace_ray_source(nonexistant, src3));

    EXPECT_TRUE(my_sim.replace_ray_source(id1, src3));
    EXPECT_EQ(my_sim.get_ray_source(id1), src3);
    EXPECT_EQ(my_sim.remove_ray_source(id1), 1);
}

TEST(SimulationData, ParametersInterface)
{
    SimulationData my_sim;

    // auto params = my_sim.get_simulation_parameters();
    const SimulationParameters &params = my_sim.get_simulation_parameters();
    SimulationParameters &mutable_params = my_sim.get_simulation_parameters();

    const int NRAYS = 10;
    my_sim.set_number_of_rays(NRAYS);
    EXPECT_EQ(my_sim.get_number_of_rays(), NRAYS);

    const double TOL = 1e-8;
    my_sim.set_tolerance(TOL);
    EXPECT_EQ(my_sim.get_tolerance(), TOL);

    const int SEED = 12345;
    my_sim.set_seed(SEED);
    EXPECT_EQ(my_sim.get_seed(), SEED);

    const double LAT = 1.25;
    my_sim.set_latitude(LAT);
    EXPECT_EQ(my_sim.get_latitude(), LAT);

    const double LONG = 0.75;
    my_sim.set_longitude(LONG);
    EXPECT_EQ(my_sim.get_longitude(), LONG);

    // TODO: Add datetime accessor tests

    EXPECT_EQ(params.seed, SEED);
    EXPECT_EQ(mutable_params.seed, SEED);
    mutable_params.seed = 0;
    EXPECT_EQ(my_sim.get_seed(), 0);
}
