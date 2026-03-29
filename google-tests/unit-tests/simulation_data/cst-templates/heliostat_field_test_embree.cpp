#include "heliostat_field_test_template.hpp"

#include <embree_runner.hpp>

using EmbreeRunnerType = SolTrace::EmbreeRunner::EmbreeRunner;

using HeliostatFieldSimulationEmbree = HeliostatFieldSimulation<EmbreeRunnerType>;

TEST_F(HeliostatFieldSimulationEmbree, singleFacet_SlantFocused)
{
    // Centerline aimpoints
    create_heliostat_field();
    setup_simData();
    simulate_check_outputs("1a", "1", "8");
    simulate_check_outputs("1a", "1", "12");

    // Scatter aimpoints
    set_scatter_aimpoints();
    simulate_check_outputs("1a", "2", "8");
    simulate_check_outputs("1a", "2", "12");
}

TEST_F(HeliostatFieldSimulationEmbree, singleFacet_BandFocused)
{
    // Centerline aimpoints
    create_heliostat_field();
    assign_focal_lengths_canting_banded();
    setup_simData();
    simulate_check_outputs("1b", "1", "8");
    simulate_check_outputs("1b", "1", "12");

    // Scatter aimpoints
    set_scatter_aimpoints();
    simulate_check_outputs("1b", "2", "8");
    simulate_check_outputs("1b", "2", "12");
}

TEST_F(HeliostatFieldSimulationEmbree, multiFacet_SlantCanted)
{
    // Centerline aimpoints
    create_heliostat_field();
    assign_canted_slant(true);      // Flat facets

    setup_simData();
    simulate_check_outputs("2a", "1", "8");
    simulate_check_outputs("2a", "1", "12");

    // Scatter aimpoints
    set_scatter_aimpoints();
    simulate_check_outputs("2a", "2", "8");
    simulate_check_outputs("2a", "2", "12");
}

TEST_F(HeliostatFieldSimulationEmbree, multiFacet_BandCanted)
{
    // Centerline aimpoints
    create_heliostat_field();
    assign_canted_banded(true);     // Flat facets

    setup_simData();
    simulate_check_outputs("2b", "1", "8");
    simulate_check_outputs("2b", "1", "12");

    // Scatter aimpoints
    set_scatter_aimpoints();
    simulate_check_outputs("2b", "2", "8");
    simulate_check_outputs("2b", "2", "12");
}

TEST_F(HeliostatFieldSimulationEmbree, multiFacet_SlantFocused_SlantCanted)
{
    // Center aimpoints;
    create_heliostat_field();
    assign_canted_slant(false);     // Slant focused (default)

    setup_simData();
    simulate_check_outputs("3a", "1", "8");
    simulate_check_outputs("3a", "1", "12");

    // Scatter aimpoints
    set_scatter_aimpoints();
    simulate_check_outputs("3a", "2", "8");
    simulate_check_outputs("3a", "2", "12");
}

TEST_F(HeliostatFieldSimulationEmbree, multiFacet_BandFocused_BandCanted)
{
    // Center aimpoints
    create_heliostat_field();
    assign_canted_banded(false);    // Canted by band
    assign_focal_lengths_banded();  // Focused by band

    setup_simData();
    simulate_check_outputs("3b", "1", "8");
    simulate_check_outputs("3b", "1", "12");

    // Scatter aimpoints
    set_scatter_aimpoints();
    simulate_check_outputs("3b", "2", "8");
    simulate_check_outputs("3b", "2", "12");
}
