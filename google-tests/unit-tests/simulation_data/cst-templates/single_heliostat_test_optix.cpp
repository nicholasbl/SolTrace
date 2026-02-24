#include <optix_runner.hpp>
#include <native_runner.hpp>
#include "single_heliostat_test_template.hpp"

using SolTrace::NativeRunner::NativeRunner;
using OptixRunnerType = OptixRunner;
using NativeRunnerType = NativeRunner;

using SingleHeliostatSimulationNative = SingleHeliostatSimulation<NativeRunnerType>;
using SingleHeliostatSimulationOptix = SingleHeliostatSimulation<OptixRunnerType>;

TEST_F(SingleHeliostatSimulationOptix, SingleFacetFlat_North)
{
    setup_simData();
    simulate_check_outputs("1a", "N");
    EXPECT_NEAR(sun_width, 15.4557, 1.e-4);
    EXPECT_NEAR(sun_height, 15.4557, 1.e-4);
}

TEST(SingleHeliostatSimulationNativeOptixComparison, SingleFacetFlat_North)
{
    int N_rays = 100000;
    double err_frac = 0.005;
    double err_abs = err_frac * (double)N_rays;


    // Run Native
    SingleHeliostatSimulationHelper<NativeRunner> sim_native;
    sim_native.runner.disable_stages(); // Disable stages
    sim_native.use_optical_errors = false;
    sim_native.use_sunshape_errors = false;
    sim_native.initialize();
    sim_native.setup_simData();
    sim_native.update_simulation_geometry(sim_native.solar_azimuth, sim_native.solar_elevation);
    SimulationResult result_native;
    sim_native.simulate(&result_native, N_rays);
    sim_native.calculate_ray_counts(result_native);

    // Run Optix
    SingleHeliostatSimulationHelper<OptixRunner> sim_optix;
    sim_optix.use_optical_errors = false;
    sim_optix.use_sunshape_errors = false;
    sim_optix.initialize();
    sim_optix.setup_simData();
    sim_optix.update_simulation_geometry(sim_optix.solar_azimuth, sim_optix.solar_elevation);
    SimulationResult result_optix;
    sim_optix.simulate(&result_optix, N_rays);
    sim_optix.calculate_ray_counts(result_optix);

    // Compare
    ASSERT_EQ(result_native.get_number_of_records(), result_optix.get_number_of_records());
    EXPECT_NEAR(sim_native.helio_hit_count, sim_optix.helio_hit_count, err_abs);
    EXPECT_NEAR(sim_native.reflect_count, sim_optix.reflect_count, err_abs);
    EXPECT_NEAR(sim_native.helio_absorb_count, sim_optix.helio_absorb_count, err_abs);
    EXPECT_NEAR(sim_native.rec_absorb_count, sim_optix.rec_absorb_count, err_abs);
    EXPECT_NEAR(sim_native.rec_hit_count, sim_optix.rec_hit_count, err_abs);
    EXPECT_NEAR(sim_native.rec_direct_hit_count, sim_optix.rec_direct_hit_count, err_abs);
    EXPECT_NEAR(sim_native.rec_via_helio_hit_count, sim_optix.rec_via_helio_hit_count, err_abs);
    
    int sun_count_native = sim_native.sun_ray_count;
    int sun_count_optix = sim_optix.sun_ray_count; 

    //EXPECT_NEAR(sim_native.miss_count, sim_optix.miss_count, 1000);

    // Add check for helio refl+absorbed add up
    // Add check for helio reflectivity
    // Add check for rec direct vs helio add up

    // Compare power per ray and sun area

    double n_records = (double)result_native.get_number_of_records();
    EXPECT_NEAR((double)sim_native.helio_hit_count / n_records, (double)sim_optix.helio_hit_count / n_records, err_frac);
    EXPECT_NEAR((double)sim_native.reflect_count / n_records, (double)sim_optix.reflect_count / n_records, err_frac);
    EXPECT_NEAR((double)sim_native.helio_absorb_count / n_records, (double)sim_optix.helio_absorb_count / n_records, err_frac);
    EXPECT_NEAR((double)sim_native.rec_absorb_count / n_records, (double)sim_optix.rec_absorb_count / n_records, err_frac);
    //EXPECT_NEAR((double)sim_native.miss_count / n_records, (double)sim_optix.miss_count / n_records, 0.1);



    int x = 0;
    
}
