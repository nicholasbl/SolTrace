#include <gtest/gtest.h>

#include <plane_reflector.hpp>
#include <simulation_data.hpp>

TEST(SimulationData, Interface)
{
    auto my_reflector = std::make_shared<PlaneReflector>();
    SimulationData my_sim;

    auto id = my_sim.add_element(my_reflector);
}

