#include <optix_runner.hpp>
#include <native_runner.hpp>
#include "single_heliostat_test_template.hpp"

using SolTrace::NativeRunner::NativeRunner;
using OptixRunnerType = OptixRunner;
using NativeRunnerType = NativeRunner;

constexpr int N_rays_glob = 2e6;
constexpr int seed = 123;
constexpr bool save = false;



static void write_to_dict(std::string key_name, double val_native,
    double val_optix, std::map<std::string, double>& dict_native,
    std::map<std::string, double>& dict_optix)
{
    dict_native[key_name] = val_native;
    dict_optix[key_name] = val_optix;
}

static void CompareRunners(SingleHeliostatSimulationHelper<NativeRunner>& sim_native,
    SingleHeliostatSimulationHelper<OptixRunner>& sim_optix, int N_rays)
{
    double err_frac = 0.01;
    double err_abs = err_frac * (double)N_rays;

    // Run cases
    sim_native.seed = seed;
    sim_native.sun_gen_type = SolTrace::Data::GenType::HALTON;
    sim_native.setup_simData();
    sim_native.update_simulation_geometry(sim_native.solar_azimuth, sim_native.solar_elevation);
    SimulationResult result_native;
    sim_native.simulate(&result_native, N_rays);
    sim_native.calculate_ray_counts(result_native);
    sim_native.calculate_sun_size(result_native);
    sim_native.read_expected_all_results("1a", "N");
    sim_native.calculate_receiver_flux_map(result_native, 30, 30, false);

    sim_optix.seed = seed;
    sim_optix.sun_gen_type = SolTrace::Data::GenType::HALTON;
    sim_optix.setup_simData();
    sim_optix.update_simulation_geometry(sim_optix.solar_azimuth, sim_optix.solar_elevation);
    SimulationResult result_optix;
    sim_optix.simulate(&result_optix, N_rays);
    sim_optix.calculate_ray_counts(result_optix);
    sim_optix.calculate_sun_size(result_optix);
    sim_optix.read_expected_all_results("1a", "N");
    sim_optix.calculate_receiver_flux_map(result_optix, 30, 30, false);

    std::map<std::string, double> dict_native;
    std::map<std::string, double> dict_optix;
    if (save)
    {
        std::string file_fluxmap_native = "native_flux_" + SolTrace::Data::GenTypeMap.at(sim_native.sun_gen_type)
            + "_" + std::to_string(int(N_rays / 1e3)) + "k.csv";
        sim_native.save_flux_map_to_file(file_fluxmap_native);

        std::string file_fluxmap_optix = "optix_flux_" + SolTrace::Data::GenTypeMap.at(sim_optix.sun_gen_type)
            + "_" + std::to_string(int(N_rays / 1e3)) + "k.csv";
        sim_optix.save_flux_map_to_file(file_fluxmap_optix);
    }

    // Compare
    ASSERT_EQ(result_native.get_number_of_records(), result_optix.get_number_of_records());
    EXPECT_NEAR(sim_native.helio_hit_count, sim_optix.helio_hit_count, err_abs);
    EXPECT_NEAR(sim_native.reflect_count, sim_optix.reflect_count, err_abs);
    EXPECT_NEAR(sim_native.helio_absorb_count, sim_optix.helio_absorb_count, err_abs);
    EXPECT_NEAR(sim_native.rec_absorb_count, sim_optix.rec_absorb_count, err_abs);
    EXPECT_NEAR(sim_native.rec_hit_count, sim_optix.rec_hit_count, err_abs);
    EXPECT_NEAR(sim_native.rec_direct_hit_count, sim_optix.rec_direct_hit_count, err_abs);
    EXPECT_NEAR(sim_native.rec_via_helio_hit_count, sim_optix.rec_via_helio_hit_count, err_abs);

    write_to_dict("00_helio_hit_count", sim_native.helio_hit_count, sim_optix.helio_hit_count, dict_native, dict_optix);
    write_to_dict("01_reflect_count", sim_native.reflect_count, sim_optix.reflect_count, dict_native, dict_optix);
    write_to_dict("02_helio_absorb_count", sim_native.helio_absorb_count, sim_optix.helio_absorb_count, dict_native, dict_optix);
    write_to_dict("03_rec_absorb_count", sim_native.rec_absorb_count, sim_optix.rec_absorb_count, dict_native, dict_optix);
    write_to_dict("04_rec_hit_count", sim_native.rec_hit_count, sim_optix.rec_hit_count, dict_native, dict_optix);
    write_to_dict("05_rec_direct_hit_count", sim_native.rec_direct_hit_count, sim_optix.rec_direct_hit_count, dict_native, dict_optix);
    write_to_dict("06_rec_via_helio_hit_count", sim_native.rec_via_helio_hit_count, sim_optix.rec_via_helio_hit_count, dict_native, dict_optix);

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

    write_to_dict("07_reflectivity", reflectivity_native, reflectivity_optix, dict_native, dict_optix);

    int sun_count_native = sim_native.sun_ray_count;
    int sun_count_optix = sim_optix.sun_ray_count;

    write_to_dict("08_sun_count", sun_count_native, sun_count_optix, dict_native, dict_optix);

    // Fraction of hits after helio
    double frac_rec_via_helio_native = (double)sim_native.rec_via_helio_hit_count / (double)sim_native.reflect_count;
    double frac_rec_via_helio_optix = (double)sim_optix.rec_via_helio_hit_count / (double)sim_optix.reflect_count;
    EXPECT_NEAR(frac_rec_via_helio_native, frac_rec_via_helio_optix, err_frac);

    write_to_dict("09_frac_rec_via_helio", frac_rec_via_helio_native, frac_rec_via_helio_optix, dict_native, dict_optix);

    // Compare power per ray
    double power_tol = (5. / (double)N_rays) * 1e3;
    EXPECT_NEAR(sim_native.power_per_ray, sim_optix.power_per_ray, power_tol);

    write_to_dict("10_power_per_ray", sim_native.power_per_ray, sim_optix.power_per_ray, dict_native, dict_optix);

    std::cerr << "Ray Count: " << sim_native.reflect_count << std::endl;

    // Total power absorbed
    double tol = 8.e-3;
    double total_power_native = (double)sim_native.rec_absorb_count * sim_native.power_per_ray * 1.e-3; // [kW]
    double total_power_optix = (double)sim_optix.rec_absorb_count * sim_optix.power_per_ray * 1.e-3; // [kW]
    EXPECT_NEAR(total_power_native, total_power_optix, tol * sim_native.expected_power);
    double total_power_diff = total_power_optix - total_power_native;
    double total_power_diff_frac = total_power_diff / sim_native.expected_power;

    write_to_dict("11_total_power", total_power_native, total_power_optix, dict_native, dict_optix);

    // Peak flux
    double peak_tol = 0.25;
    double expected_peak_flux = sim_native.expected_peak_flux;
    double peak_flux_native = sim_native.PeakFlux / 1.e3;
    double peak_flux_optix = sim_optix.PeakFlux / 1.e3;
    EXPECT_NEAR(peak_flux_native, peak_flux_optix, peak_tol * sim_native.expected_peak_flux);
    double peak_flux_diff = peak_flux_optix - peak_flux_native;
    double peak_flux_diff_frac = peak_flux_diff / sim_native.expected_peak_flux;

    write_to_dict("12_peak_flux", peak_flux_native, peak_flux_optix, dict_native, dict_optix);

    double centroid0_native = sim_native.Centroid[0];
    double centroid0_optix = sim_optix.Centroid[0];
    double centroid1_native = sim_native.Centroid[1];
    double centroid1_optix = sim_optix.Centroid[1];

    write_to_dict("13_centroid0", centroid0_native, centroid0_optix, dict_native, dict_optix);
    write_to_dict("14_centroid1", centroid1_native, centroid1_optix, dict_native, dict_optix);

    write_to_dict("15_sigmaflux", sim_native.SigmaFlux, sim_optix.SigmaFlux, dict_native, dict_optix);

    // RMS
    sim_native.calculate_receiver_flux_map(result_native, 100, 150, false);  // Re-calculate for low-accuracy runs
    sim_optix.calculate_receiver_flux_map(result_optix, 100, 150, false);
    EXPECT_EQ(sim_native.fluxGrid.nrows(), sim_optix.fluxGrid.nrows());
    EXPECT_EQ(sim_native.fluxGrid.ncols(), sim_optix.fluxGrid.ncols());
    double rmse = 0.0;
    double average_flux_native = 0.0;
    double average_flux_optix = 0.0;
    for (size_t r = 0; r < sim_native.fluxGrid.nrows(); r++) {
        for (size_t c = 0; c < sim_native.fluxGrid.ncols(); c++) {
            double flux_native = sim_native.fluxGrid.at(r, c) * sim_native.zScale / 1.e3;
            double flux_optix = sim_optix.fluxGrid.at(r, c) * sim_optix.zScale / 1.e3;
            rmse += pow(flux_native - flux_optix, 2);
            average_flux_native += flux_native;
            average_flux_optix += flux_optix;
        }
    }

    // RMS
    rmse = sqrt(rmse / (sim_native.fluxGrid.nrows() * sim_native.fluxGrid.ncols()));
    double rmse_tol = 0.11; // Should be 0.11
    EXPECT_LE(rmse / (peak_flux_native), rmse_tol);

    // Average flux
    average_flux_native /= (sim_native.fluxGrid.nrows() * sim_native.fluxGrid.ncols());
    average_flux_optix /= (sim_optix.fluxGrid.nrows() * sim_optix.fluxGrid.ncols());
    EXPECT_NEAR(average_flux_native, average_flux_optix, rmse_tol);
    double average_flux_diff = average_flux_optix - average_flux_native;
    double average_flux_diff_frac = average_flux_diff / average_flux_native;

    write_to_dict("16_average_flux", average_flux_native, average_flux_optix, dict_native, dict_optix);

    if (save)
    {
        std::string file_outputs_native = "native_outputs_" + SolTrace::Data::GenTypeMap.at(sim_native.sun_gen_type)
            + "_" + std::to_string(int(N_rays / 1e3)) + "k.csv";
        sim_native.save_outputs(file_outputs_native, dict_native);

        std::string file_outputs_optix = "optix_outputs_" + SolTrace::Data::GenTypeMap.at(sim_optix.sun_gen_type)
            + "_" + std::to_string(int(N_rays / 1e3)) + "k.csv";
        sim_optix.save_outputs(file_outputs_optix, dict_optix);
    }
    

    int x = 0;
}


TEST(SingleHeliostatOptixNative, SingleFacetFlat_North)
{
    // Make native
    SingleHeliostatSimulationHelper<NativeRunner> sim_native;
    sim_native.runner.disable_stages();
    sim_native.runner.set_number_of_threads(10);
    sim_native.initialize();

    // Make optix
    SingleHeliostatSimulationHelper<OptixRunner> sim_optix;
    sim_optix.initialize();

    CompareRunners(sim_native, sim_optix, N_rays_glob);
}

TEST(SingleHeliostatOptixNative, SingleFacetFlat_Southeast)
{
    // Make native
    SingleHeliostatSimulationHelper<NativeRunner> sim_native;
    sim_native.runner.disable_stages();
    sim_native.runner.set_number_of_threads(10);
    //sim_native.use_optical_errors = false;
    //sim_native.use_sunshape_errors = false;
    sim_native.initialize();
    sim_native.set_heliostat_to_southeast();

    // Make optix
    SingleHeliostatSimulationHelper<OptixRunner> sim_optix;
    //sim_optix.use_optical_errors = false;
    //sim_optix.use_sunshape_errors = false;
    sim_optix.initialize();
    sim_optix.set_heliostat_to_southeast();

    CompareRunners(sim_native, sim_optix, N_rays_glob);
}

TEST(SingleHeliostatOptixNative, SingleFacetFocused_North)
{
    // Make native
    SingleHeliostatSimulationHelper<NativeRunner> sim_native;
    sim_native.runner.disable_stages();
    sim_native.runner.set_number_of_threads(10);
    sim_native.initialize();
    sim_native.set_slant_focal_length();

    // Make optix
    SingleHeliostatSimulationHelper<OptixRunner> sim_optix;
    sim_optix.initialize();
    sim_optix.set_slant_focal_length();

    CompareRunners(sim_native, sim_optix, N_rays_glob);
}

TEST(SingleHeliostatOptixNative, SingleFacetFocused_Southeast)
{
    // Make native
    SingleHeliostatSimulationHelper<NativeRunner> sim_native;
    sim_native.runner.disable_stages();
    sim_native.runner.set_number_of_threads(10);
    sim_native.initialize();
    sim_native.set_heliostat_to_southeast();
    sim_native.set_slant_focal_length();

    // Make optix
    SingleHeliostatSimulationHelper<OptixRunner> sim_optix;
    sim_optix.initialize();
    sim_optix.set_heliostat_to_southeast();
    sim_optix.set_slant_focal_length();

    CompareRunners(sim_native, sim_optix, N_rays_glob);
}

TEST(SingleHeliostatOptixNative, MultiFacetFlat_NoCanting_North)
{
    // Make native
    SingleHeliostatSimulationHelper<NativeRunner> sim_native;
    sim_native.runner.disable_stages();
    sim_native.runner.set_number_of_threads(10);
    sim_native.initialize();
    sim_native.set_flat_multi_facet();

    // Make optix
    SingleHeliostatSimulationHelper<OptixRunner> sim_optix;
    sim_optix.initialize();
    sim_optix.set_flat_multi_facet();

    CompareRunners(sim_native, sim_optix, N_rays_glob);
}

TEST(SingleHeliostatOptixNative, MultiFacetFlat_NoCanting_Southeast)
{
    // Make native
    SingleHeliostatSimulationHelper<NativeRunner> sim_native;
    sim_native.runner.disable_stages();
    sim_native.runner.set_number_of_threads(10);
    sim_native.initialize();
    sim_native.set_flat_multi_facet();
    sim_native.set_heliostat_to_southeast();

    // Make optix
    SingleHeliostatSimulationHelper<OptixRunner> sim_optix;
    sim_optix.initialize();
    sim_optix.set_flat_multi_facet();
    sim_optix.set_heliostat_to_southeast();

    CompareRunners(sim_native, sim_optix, N_rays_glob);
}

TEST(SingleHeliostatOptixNative, MultiFacetFlat_SlantCanting_North)
{
    // Make native
    SingleHeliostatSimulationHelper<NativeRunner> sim_native;
    sim_native.runner.disable_stages();
    sim_native.runner.set_number_of_threads(10);
    sim_native.initialize();
    sim_native.set_onaxis_slant_canting();

    // Make optix
    SingleHeliostatSimulationHelper<OptixRunner> sim_optix;
    sim_optix.initialize();
    sim_optix.set_onaxis_slant_canting();

    CompareRunners(sim_native, sim_optix, N_rays_glob);
}

TEST(SingleHeliostatOptixNative, MultiFacetFlat_SlantCanting_Southeast)
{
    // Make native
    SingleHeliostatSimulationHelper<NativeRunner> sim_native;
    sim_native.runner.disable_stages();
    sim_native.runner.set_number_of_threads(10);
    sim_native.initialize();
    sim_native.set_heliostat_to_southeast();
    sim_native.set_onaxis_slant_canting();

    // Make optix
    SingleHeliostatSimulationHelper<OptixRunner> sim_optix;
    sim_optix.initialize();
    sim_optix.set_heliostat_to_southeast();
    sim_optix.set_onaxis_slant_canting();

    CompareRunners(sim_native, sim_optix, N_rays_glob);
}

TEST(SingleHeliostatOptixNative, MultiFacetFocused_SlantCanting_North)
{
    // Make native
    SingleHeliostatSimulationHelper<NativeRunner> sim_native;
    sim_native.runner.disable_stages();
    sim_native.runner.set_number_of_threads(10);
    sim_native.initialize();
    sim_native.set_slant_focal_length();
    sim_native.set_onaxis_slant_canting();

    // Make optix
    SingleHeliostatSimulationHelper<OptixRunner> sim_optix;
    sim_optix.initialize();
    sim_optix.set_slant_focal_length();
    sim_optix.set_onaxis_slant_canting();

    CompareRunners(sim_native, sim_optix, N_rays_glob);
}

TEST(SingleHeliostatOptixNative, MultiFacetFocused_SlantCanting_Southeast)
{
    // Make native
    SingleHeliostatSimulationHelper<NativeRunner> sim_native;
    sim_native.runner.disable_stages();
    sim_native.runner.set_number_of_threads(10);
    sim_native.initialize();
    sim_native.set_heliostat_to_southeast();
    sim_native.set_slant_focal_length();
    sim_native.set_onaxis_slant_canting();

    // Make optix
    SingleHeliostatSimulationHelper<OptixRunner> sim_optix;
    sim_optix.initialize();
    sim_optix.set_heliostat_to_southeast();
    sim_optix.set_slant_focal_length();
    sim_optix.set_onaxis_slant_canting();

    CompareRunners(sim_native, sim_optix, N_rays_glob);
}

TEST(SingleHeliostatOptixNative, NoErrors)
{
    bool use_optical = false;
    bool use_sunshape = false;

    // Make native
    SingleHeliostatSimulationHelper<NativeRunner> sim_native;
    sim_native.runner.disable_stages(); // Disable stages
    sim_native.use_optical_errors = use_optical;
    sim_native.use_sunshape_errors = use_sunshape;
    sim_native.runner.set_number_of_threads(10);
    sim_native.initialize();
    
    // Make optix
    SingleHeliostatSimulationHelper<OptixRunner> sim_optix;
    sim_optix.use_optical_errors = use_optical;
    sim_optix.use_sunshape_errors = use_sunshape;
    sim_optix.initialize();

    CompareRunners(sim_native, sim_optix, N_rays_glob);
}

TEST(SingleHeliostatOptixNative, SunShapePillBoxOnly)
{
    bool use_optical = false;
    bool use_sunshape = true;

    // Make native
    SingleHeliostatSimulationHelper<NativeRunner> sim_native;
    sim_native.runner.disable_stages(); // Disable stages
    sim_native.use_optical_errors = use_optical;
    sim_native.use_sunshape_errors = use_sunshape;
    sim_native.runner.set_number_of_threads(10);
    sim_native.initialize();

    // Make optix
    SingleHeliostatSimulationHelper<OptixRunner> sim_optix;
    sim_optix.use_optical_errors = use_optical;
    sim_optix.use_sunshape_errors = use_sunshape;
    sim_optix.initialize();

    CompareRunners(sim_native, sim_optix, N_rays_glob);
}

TEST(SingleHeliostatOptixNative, SunShapeGaussOnly)
{
    bool use_optical = false;
    bool use_sunshape = true;

    // Make native
    SingleHeliostatSimulationHelper<NativeRunner> sim_native;
    sim_native.runner.disable_stages(); // Disable stages
    sim_native.use_optical_errors = use_optical;
    sim_native.use_sunshape_errors = use_sunshape;
    sim_native.sun_shape = SunShape::GAUSSIAN;
    sim_native.gauss_sigma = 2;
    sim_native.runner.set_number_of_threads(10);
    sim_native.initialize();

    // Make optix
    SingleHeliostatSimulationHelper<OptixRunner> sim_optix;
    sim_optix.use_optical_errors = use_optical;
    sim_optix.use_sunshape_errors = use_sunshape;
    sim_optix.sun_shape = SunShape::GAUSSIAN;
    sim_optix.gauss_sigma = 2;
    sim_optix.initialize();

    CompareRunners(sim_native, sim_optix, N_rays_glob);
}

TEST(SingleHeliostatOptixNative, SlopeGaussOnly)
{
    bool use_optical = true;
    bool use_sunshape = false;

    // Make native
    SingleHeliostatSimulationHelper<NativeRunner> sim_native;
    sim_native.runner.disable_stages(); // Disable stages
    sim_native.use_optical_errors = use_optical;
    sim_native.use_sunshape_errors = use_sunshape;
    sim_native.runner.set_number_of_threads(10);
    sim_native.initialize();

    // Make optix
    SingleHeliostatSimulationHelper<OptixRunner> sim_optix;
    sim_optix.use_optical_errors = use_optical;
    sim_optix.use_sunshape_errors = use_sunshape;
    sim_optix.initialize();

    CompareRunners(sim_native, sim_optix, N_rays_glob);
}

TEST(SingleHeliostatOptixNative, SlopePillBoxOnly)
{
    bool use_optical = true;
    bool use_sunshape = false;

    // Make native
    SingleHeliostatSimulationHelper<NativeRunner> sim_native;
    sim_native.runner.disable_stages(); // Disable stages
    sim_native.use_optical_errors = use_optical;
    sim_native.use_sunshape_errors = use_sunshape;
    sim_native.error_dist = DistributionType::PILLBOX;
    sim_native.runner.set_number_of_threads(10);
    sim_native.initialize();

    // Make optix
    SingleHeliostatSimulationHelper<OptixRunner> sim_optix;
    sim_optix.use_optical_errors = use_optical;
    sim_optix.use_sunshape_errors = use_sunshape;
    sim_optix.error_dist = DistributionType::PILLBOX;
    sim_optix.initialize();

    CompareRunners(sim_native, sim_optix, N_rays_glob);
}

TEST(SingleHeliostatOptixNative, SunShapeAndSlope)
{
    bool use_optical = true;
    bool use_sunshape = true;

    // Make native
    SingleHeliostatSimulationHelper<NativeRunner> sim_native;
    sim_native.runner.disable_stages(); // Disable stages
    sim_native.use_optical_errors = use_optical;
    sim_native.use_sunshape_errors = use_sunshape;
    sim_native.runner.set_number_of_threads(10);
    sim_native.initialize();

    // Make optix
    SingleHeliostatSimulationHelper<OptixRunner> sim_optix;
    sim_optix.use_optical_errors = use_optical;
    sim_optix.use_sunshape_errors = use_sunshape;
    sim_optix.initialize();

    CompareRunners(sim_native, sim_optix, N_rays_glob);
}

TEST(SingleHeliostatOptixNative, SpecGaussOnly)
{
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
    sim_native.initialize();

    // Make optix
    SingleHeliostatSimulationHelper<OptixRunner> sim_optix;
    sim_optix.use_optical_errors = use_optical;
    sim_optix.use_sunshape_errors = use_sunshape;
    sim_optix.slope_error = 0;     // Turn off slope error
    sim_optix.spec_error = 2;    // Set specularity error
    sim_optix.initialize();

    CompareRunners(sim_native, sim_optix, N_rays_glob);
}

TEST(SingleHeliostatOptixNative, SpecPillBoxOnly)
{
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
    sim_native.initialize();

    // Make optix
    SingleHeliostatSimulationHelper<OptixRunner> sim_optix;
    sim_optix.use_optical_errors = use_optical;
    sim_optix.use_sunshape_errors = use_sunshape;
    sim_optix.slope_error = 0;      // Turn off slope error
    sim_optix.spec_error = 2;       // Set specularity error
    sim_optix.error_dist = DistributionType::PILLBOX;
    sim_optix.initialize();

    CompareRunners(sim_native, sim_optix, N_rays_glob);
}

TEST(SingleHeliostatOptixNative, SunShapeSlopeAndSpec)
{
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
    sim_native.initialize();

    // Make optix
    SingleHeliostatSimulationHelper<OptixRunner> sim_optix;
    sim_optix.use_optical_errors = use_optical;
    sim_optix.use_sunshape_errors = use_sunshape;
    sim_optix.slope_error = 2;     // Turn off slope error
    sim_optix.spec_error = 2;    // Set specularity error
    sim_optix.initialize();

    CompareRunners(sim_native, sim_optix, N_rays_glob);
}