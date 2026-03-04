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
    //simulate_check_outputs("1a", "N");
    //EXPECT_NEAR(sun_width, 15.4557, 1.e-4);
    //EXPECT_NEAR(sun_height, 15.4557, 1.e-4);
}

void CompareRunners(SingleHeliostatSimulationHelper<NativeRunner>& sim_native,
    SingleHeliostatSimulationHelper<OptixRunner>& sim_optix, const int N_rays = 1e5)
{
    double err_frac = 0.01;
    double err_abs = err_frac * (double)N_rays;

    // Run cases
    sim_native.initialize();
    sim_native.setup_simData();
    sim_native.update_simulation_geometry(sim_native.solar_azimuth, sim_native.solar_elevation);
    SimulationResult result_native;
    sim_native.simulate(&result_native, N_rays);
    sim_native.calculate_ray_counts(result_native);
    sim_native.calculate_sun_size(result_native);

    sim_optix.initialize();
    sim_optix.setup_simData();
    sim_optix.update_simulation_geometry(sim_optix.solar_azimuth, sim_optix.solar_elevation);
    SimulationResult result_optix;
    sim_optix.simulate(&result_optix, N_rays);
    sim_optix.calculate_ray_counts(result_optix);
    sim_optix.calculate_sun_size(result_optix);


    // Compare
    ASSERT_EQ(result_native.get_number_of_records(), result_optix.get_number_of_records());
    EXPECT_NEAR(sim_native.helio_hit_count, sim_optix.helio_hit_count, err_abs);
    EXPECT_NEAR(sim_native.reflect_count, sim_optix.reflect_count, err_abs);
    EXPECT_NEAR(sim_native.helio_absorb_count, sim_optix.helio_absorb_count, err_abs);
    EXPECT_NEAR(sim_native.rec_absorb_count, sim_optix.rec_absorb_count, err_abs);
    EXPECT_NEAR(sim_native.rec_hit_count, sim_optix.rec_hit_count, err_abs);
    EXPECT_NEAR(sim_native.rec_direct_hit_count, sim_optix.rec_direct_hit_count, err_abs);
    EXPECT_NEAR(sim_native.rec_via_helio_hit_count, sim_optix.rec_via_helio_hit_count, err_abs);

    // Helio rays add up
    EXPECT_EQ(sim_native.reflect_count + sim_native.helio_absorb_count, sim_native.helio_hit_count);
    EXPECT_EQ(sim_optix.reflect_count + sim_optix.helio_absorb_count, sim_optix.helio_hit_count);

    // Receiver rays add up
    EXPECT_EQ(sim_native.rec_direct_hit_count + sim_native.rec_via_helio_hit_count, sim_native.rec_hit_count);
    EXPECT_EQ(sim_optix.rec_direct_hit_count + sim_optix.rec_via_helio_hit_count, sim_optix.rec_hit_count);

    // Reflectivity
    double reflectivity_native = (double)sim_native.reflect_count / (double)sim_native.helio_hit_count;
    double reflectivity_optix = (double)sim_optix.reflect_count / (double)sim_optix.helio_hit_count;
    EXPECT_NEAR(reflectivity_native, 0.9, 0.02);
    EXPECT_NEAR(reflectivity_optix, 0.9, 0.02);

    int sun_count_native = sim_native.sun_ray_count;
    int sun_count_optix = sim_optix.sun_ray_count;

    // Fraction of hits after helio
    double frac_rec_via_helio_native = (double)sim_native.rec_via_helio_hit_count / (double)sim_native.reflect_count;
    double frac_rec_via_helio_optix = (double)sim_optix.rec_via_helio_hit_count / (double)sim_optix.reflect_count;
    EXPECT_NEAR(frac_rec_via_helio_native, frac_rec_via_helio_optix, err_frac * 5);

    // Compare power per ray
    double power_tol = (5. / (double)N_rays) * 1e3;
    EXPECT_NEAR(sim_native.power_per_ray, sim_optix.power_per_ray, power_tol);

    std::cerr << "Ray Count: " << sim_native.reflect_count << std::endl;

    int x = 0;
}

TEST(SingleHelioOptixNative, NoErrors)
{
    int N_rays = 100000;

    bool use_optical = false;
    bool use_sunshape = false;

    // Make native
    SingleHeliostatSimulationHelper<NativeRunner> sim_native;
    sim_native.runner.disable_stages(); // Disable stages
    sim_native.use_optical_errors = use_optical;
    sim_native.use_sunshape_errors = use_sunshape;
    sim_native.runner.set_number_of_threads(10);
    
    // Make optix
    SingleHeliostatSimulationHelper<OptixRunner> sim_optix;
    sim_optix.use_optical_errors = use_optical;
    sim_optix.use_sunshape_errors = use_sunshape;

    CompareRunners(sim_native, sim_optix, N_rays);
}

TEST(SingleHelioOptixNative, SunShapeOnly)
{
    int N_rays = 100000;
    bool use_optical = false;
    bool use_sunshape = true;

    // Make native
    SingleHeliostatSimulationHelper<NativeRunner> sim_native;
    sim_native.runner.disable_stages(); // Disable stages
    sim_native.use_optical_errors = use_optical;
    sim_native.use_sunshape_errors = use_sunshape;
    sim_native.runner.set_number_of_threads(10);

    // Make optix
    SingleHeliostatSimulationHelper<OptixRunner> sim_optix;
    sim_optix.use_optical_errors = use_optical;
    sim_optix.use_sunshape_errors = use_sunshape;

    CompareRunners(sim_native, sim_optix, N_rays);
}

TEST(SingleHelioOptixNative, SlopeGaussOnly)
{
    int N_rays = 1e5;

    bool use_optical = true;
    bool use_sunshape = false;

    // Make native
    SingleHeliostatSimulationHelper<NativeRunner> sim_native;
    sim_native.runner.disable_stages(); // Disable stages
    sim_native.use_optical_errors = use_optical;
    sim_native.use_sunshape_errors = use_sunshape;
    sim_native.runner.set_number_of_threads(10);

    // Make optix
    SingleHeliostatSimulationHelper<OptixRunner> sim_optix;
    sim_optix.use_optical_errors = use_optical;
    sim_optix.use_sunshape_errors = use_sunshape;

    CompareRunners(sim_native, sim_optix, N_rays);
}

TEST(SingleHelioOptixNative, SlopePillBoxOnly)
{
    int N_rays = 1e5;

    bool use_optical = true;
    bool use_sunshape = false;

    // Make native
    SingleHeliostatSimulationHelper<NativeRunner> sim_native;
    sim_native.runner.disable_stages(); // Disable stages
    sim_native.use_optical_errors = use_optical;
    sim_native.use_sunshape_errors = use_sunshape;
    sim_native.error_dist = DistributionType::PILLBOX;
    sim_native.runner.set_number_of_threads(10);

    // Make optix
    SingleHeliostatSimulationHelper<OptixRunner> sim_optix;
    sim_optix.use_optical_errors = use_optical;
    sim_optix.use_sunshape_errors = use_sunshape;
    sim_optix.error_dist = DistributionType::PILLBOX;

    CompareRunners(sim_native, sim_optix, N_rays);
}

TEST(SingleHelioOptixNative, SunShapeAndSlope)
{
    int N_rays = 1e5;

    bool use_optical = true;
    bool use_sunshape = true;

    // Make native
    SingleHeliostatSimulationHelper<NativeRunner> sim_native;
    sim_native.runner.disable_stages(); // Disable stages
    sim_native.use_optical_errors = use_optical;
    sim_native.use_sunshape_errors = use_sunshape;
    sim_native.runner.set_number_of_threads(10);

    // Make optix
    SingleHeliostatSimulationHelper<OptixRunner> sim_optix;
    sim_optix.use_optical_errors = use_optical;
    sim_optix.use_sunshape_errors = use_sunshape;

    CompareRunners(sim_native, sim_optix, N_rays);
}

TEST(SingleHelioOptixNative, SpecGaussOnly)
{
    int N_rays = 1e5;

    bool use_optical = true;
    bool use_sunshape = false;

    // Make native
    SingleHeliostatSimulationHelper<NativeRunner> sim_native;
    sim_native.runner.disable_stages(); // Disable stages
    sim_native.use_optical_errors = use_optical;
    sim_native.use_sunshape_errors = use_sunshape;
    sim_native.slope_error = 0;     // Turn off slope error
    sim_native.spec_error = 2;    // Set specularity error
    sim_native.runner.set_number_of_threads(10);

    // Make optix
    SingleHeliostatSimulationHelper<OptixRunner> sim_optix;
    sim_optix.use_optical_errors = use_optical;
    sim_optix.use_sunshape_errors = use_sunshape;
    sim_optix.slope_error = 0;     // Turn off slope error
    sim_optix.spec_error = 2;    // Set specularity error

    CompareRunners(sim_native, sim_optix, N_rays);
}

TEST(SingleHelioOptixNative, SpecPillBoxOnly)
{
    int N_rays = 1e5;

    bool use_optical = true;
    bool use_sunshape = false;

    // Make native
    SingleHeliostatSimulationHelper<NativeRunner> sim_native;
    sim_native.runner.disable_stages(); // Disable stages
    sim_native.use_optical_errors = use_optical;
    sim_native.use_sunshape_errors = use_sunshape;
    sim_native.slope_error = 0;     // Turn off slope error
    sim_native.spec_error = 2;      // Set specularity error
    sim_native.error_dist = DistributionType::PILLBOX;
    sim_native.runner.set_number_of_threads(10);

    // Make optix
    SingleHeliostatSimulationHelper<OptixRunner> sim_optix;
    sim_optix.use_optical_errors = use_optical;
    sim_optix.use_sunshape_errors = use_sunshape;
    sim_optix.slope_error = 0;      // Turn off slope error
    sim_optix.spec_error = 2;       // Set specularity error
    sim_optix.error_dist = DistributionType::PILLBOX;

    CompareRunners(sim_native, sim_optix, N_rays);
}

TEST(SingleHelioOptixNative, SunShapeSlopeAndSpec)
{
    int N_rays = 1e5;

    bool use_optical = true;
    bool use_sunshape = true;

    // Make native
    SingleHeliostatSimulationHelper<NativeRunner> sim_native;
    sim_native.runner.disable_stages(); // Disable stages
    sim_native.use_optical_errors = use_optical;
    sim_native.use_sunshape_errors = use_sunshape;
    sim_native.slope_error = 2;     // Turn off slope error
    sim_native.spec_error = 2;    // Set specularity error
    sim_native.runner.set_number_of_threads(10);

    // Make optix
    SingleHeliostatSimulationHelper<OptixRunner> sim_optix;
    sim_optix.use_optical_errors = use_optical;
    sim_optix.use_sunshape_errors = use_sunshape;
    sim_optix.slope_error = 2;     // Turn off slope error
    sim_optix.spec_error = 2;    // Set specularity error

    CompareRunners(sim_native, sim_optix, N_rays);
}