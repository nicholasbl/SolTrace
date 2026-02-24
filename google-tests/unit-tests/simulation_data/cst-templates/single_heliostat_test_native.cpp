#include <native_runner.hpp>
#include "single_heliostat_test_template.hpp"

using NativeRunnerType = SolTrace::NativeRunner::NativeRunner;

using SingleHeliostatSimulationNative = SingleHeliostatSimulation<NativeRunnerType>;

const int N_threads = 10;

TEST_F(SingleHeliostatSimulationNative, SingleFacetFlat_North)
{
    this->runner.set_number_of_threads(N_threads);
    setup_simData();
    simulate_check_outputs("1a", "N");
    EXPECT_NEAR(sun_width, 15.4557, 1.e-4);
    EXPECT_NEAR(sun_height, 15.4557, 1.e-4);
}

TEST_F(SingleHeliostatSimulationNative, SingleFacetFlat_Southeast)
{
    this->runner.set_number_of_threads(N_threads);
    set_heliostat_to_southeast();
    setup_simData();
    simulate_check_outputs("1a", "SE");
    EXPECT_NEAR(sun_width, 15.4557, 1.e-4);
    EXPECT_NEAR(sun_height, 15.4557, 1.e-4);
}

TEST_F(SingleHeliostatSimulationNative, SingleFacetFocused_North)
{
    this->runner.set_number_of_threads(N_threads);
    set_slant_focal_length();
    setup_simData();
    simulate_check_outputs("1b", "N");
    EXPECT_NEAR(sun_width, 15.4557, 1.e-4);
    EXPECT_NEAR(sun_height, 15.4557, 1.e-4);
}

TEST_F(SingleHeliostatSimulationNative, SingleFacetFocused_Southeast)
{
    this->runner.set_number_of_threads(N_threads);
    set_heliostat_to_southeast();
    set_slant_focal_length();
    setup_simData();
    simulate_check_outputs("1b", "SE");
    EXPECT_NEAR(sun_width, 15.4557, 1.e-4);
    EXPECT_NEAR(sun_height, 15.4557, 1.e-4);
}

TEST_F(SingleHeliostatSimulationNative, MultiFacetFlat_NoCanting_North)
{
    this->runner.set_number_of_threads(N_threads);
    set_flat_multi_facet();
    setup_simData();
    simulate_check_outputs("2", "N");
    EXPECT_NEAR(sun_width, 12.4214, 1.e-4);
    EXPECT_NEAR(sun_height, 10.4195, 1.e-4);
}

TEST_F(SingleHeliostatSimulationNative, MultiFacetFlat_NoCanting_Southeast)
{
    this->runner.set_number_of_threads(N_threads);
    set_flat_multi_facet();
    set_heliostat_to_southeast();
    setup_simData();
    simulate_check_outputs("2", "SE");
    EXPECT_NEAR(sun_width, 11.8574, 1.e-3);
    EXPECT_NEAR(sun_height, 11.5183, 1.e-3);
}

TEST_F(SingleHeliostatSimulationNative, MultiFacetFlat_SlantCanting_North)
{
    this->runner.set_number_of_threads(N_threads);
    set_onaxis_slant_canting();
    setup_simData();
    simulate_check_outputs("3", "N");
    EXPECT_NEAR(sun_width, 12.4214, 1.e-4);
    EXPECT_NEAR(sun_height, 10.4236, 1.e-4);
}

TEST_F(SingleHeliostatSimulationNative, MultiFacetFlat_SlantCanting_Southeast)
{
    this->runner.set_number_of_threads(N_threads);
    set_heliostat_to_southeast();
    set_onaxis_slant_canting();
    setup_simData();
    simulate_check_outputs("3", "SE");
    EXPECT_NEAR(sun_width, 11.8574, 1.e-3);
    EXPECT_NEAR(sun_height, 11.5183, 1.e-3);
}

TEST_F(SingleHeliostatSimulationNative, MultiFacetFocused_SlantCanting_North)
{
    this->runner.set_number_of_threads(N_threads);
    set_slant_focal_length();
    set_onaxis_slant_canting();
    setup_simData();
    simulate_check_outputs("4", "N");
    EXPECT_NEAR(sun_width, 12.4214, 1.e-4);
    EXPECT_NEAR(sun_height, 10.4236, 1.e-4);
}

TEST_F(SingleHeliostatSimulationNative, MultiFacetFocused_SlantCanting_Southeast)
{
    this->runner.set_number_of_threads(N_threads);
    set_heliostat_to_southeast();
    set_slant_focal_length();
    set_onaxis_slant_canting();
    setup_simData();
    simulate_check_outputs("4", "SE");
    EXPECT_NEAR(sun_width, 11.8574, 1.e-3);
    EXPECT_NEAR(sun_height, 11.5183, 1.e-3);
}
