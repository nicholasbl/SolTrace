#include <gtest/gtest.h>

#include <single_element.hpp>
#include <simulation_data.hpp>

TEST(SimulationData, Interface)
{
    // auto my_reflector = std::make_shared<PlaneReflector>();
    auto my_reflector = make_element<SingleElement>();
    SimulationData my_sim;

    auto id = my_sim.add_element(my_reflector);
}

