
#include <gtest/gtest.h>

#include <aperture.hpp>
#include <constants.hpp>
#include <native_runner.hpp>
#include <simulation_data_export.hpp>
#include <simulation_result_export.hpp>

#include "common.hpp"
TEST(io_json, json_round_trip)
{
    namespace fs = std::filesystem;
    using json = nlohmann::ordered_json;

    // Build paths
    const fs::path project_root(PROJECT_DIR);
    const fs::path sample_path = project_root / "High Flux Solar Furnace.stinput";
    const fs::path output_path_1 = project_root / "json_test_1.json";
    const fs::path output_path_2 = project_root / "json_test_2.json";

    ASSERT_TRUE(fs::exists(sample_path)) << "Sample .stinput not found: " << sample_path.string();

    // Load original simulation from .stinput
    SimulationData sd_original;
    ASSERT_TRUE(sd_original.import_from_file(sample_path.string())) << "Failed to import stinput";

    // Export to JSON
    ASSERT_NO_THROW(sd_original.export_json_file(output_path_1.string())) << "Failed to export first JSON";

    // Import from first JSON
    SimulationData sd_round_trip;
    ASSERT_NO_THROW(sd_round_trip.import_json_file(output_path_1.string())) << "Failed to import JSON";

    // Re-export second JSON
    ASSERT_NO_THROW(sd_round_trip.export_json_file(output_path_2.string())) << "Failed to export second JSON";

    // Parse both JSON files
    std::ifstream ifs1(output_path_1);
    std::ifstream ifs2(output_path_2);
    ASSERT_TRUE(ifs1.is_open()) << "Cannot open first JSON file";
    ASSERT_TRUE(ifs2.is_open()) << "Cannot open second JSON file";

    json root1;
    json root2;
    try {
        root1 = json::parse(ifs1);
        root2 = json::parse(ifs2);
    }
    catch (const std::exception& e) {
        FAIL() << "JSON parse failure: " << e.what();
    }

    // Basic schema/version checks
    ASSERT_TRUE(root1.contains("schema_version"));
    ASSERT_TRUE(root2.contains("schema_version"));
    EXPECT_EQ(root1["schema_version"], root2["schema_version"]);

    // Simulation parameters
    ASSERT_TRUE(root1.contains("simulation_parameters"));
    ASSERT_TRUE(root2.contains("simulation_parameters"));
    EXPECT_EQ(root1["simulation_parameters"], root2["simulation_parameters"]);

    // Ray sources
    ASSERT_TRUE(root1.contains("ray_sources"));
    ASSERT_TRUE(root2.contains("ray_sources"));
    EXPECT_EQ(root1["ray_sources"], root2["ray_sources"]);

    // Elements
    ASSERT_TRUE(root1.contains("elements"));
    ASSERT_TRUE(root2.contains("elements"));
    EXPECT_EQ(root1["elements"], root2["elements"]);

    // Global sanity checks against SimulationData instances
    EXPECT_EQ(root1["number_of_elements"], root2["number_of_elements"]);
    EXPECT_EQ(sd_original.get_number_of_elements(), sd_round_trip.get_number_of_elements());
    EXPECT_EQ(sd_original.get_number_of_ray_sources(), sd_round_trip.get_number_of_ray_sources());

    // Full structural equality
    ASSERT_TRUE(root1 == root2);

    // Conditional cleanup: remove only if test passed so far.
    if (!::testing::Test::HasFailure()) {
        std::error_code ec;
        fs::remove(output_path_1, ec);
        fs::remove(output_path_2, ec);
    }
}

TEST(io_json, large_field_comparison)
{
    namespace fs = std::filesystem;
    using json = nlohmann::ordered_json;

    // Build paths
    const fs::path project_root(PROJECT_DIR);
    const fs::path sample_path = project_root / "Power-tower-surround_singlefacet_large_afternoon.stinput";
    const fs::path output_path = project_root / "number_elements_test.json";

    ASSERT_TRUE(fs::exists(sample_path)) << "Sample .stinput not found: " << sample_path.string();

    // Load original simulation from .stinput
    SimulationData sd_original;
    ASSERT_TRUE(sd_original.import_from_file(sample_path.string())) << "Failed to import stinput";

    // Export to JSON
    ASSERT_NO_THROW(sd_original.export_json_file(output_path.string())) << "Failed to export first JSON";

    // Import from first JSON
    SimulationData sd_round_trip;
    ASSERT_NO_THROW(sd_round_trip.import_json_file(output_path.string())) << "Failed to import JSON";

    // Compare number of elements
    int N_elements_original = sd_original.get_number_of_elements();
    int N_elements_round_trip = sd_round_trip.get_number_of_elements();
    ASSERT_TRUE(N_elements_original == N_elements_round_trip) << "Element number is not equal";

    // Compare number of sources
    int N_sources_original = sd_original.get_number_of_ray_sources();
    int N_sources_round_trip = sd_round_trip.get_number_of_ray_sources();
    ASSERT_TRUE(N_sources_original == N_sources_round_trip) << "Ray sources number is not equal";

    // Compare number of rays
    int N_rays_original = sd_original.get_number_of_rays();
    int N_rays_round_trip = sd_round_trip.get_number_of_rays();
    ASSERT_TRUE(N_rays_original == N_rays_round_trip) << "Ray number is not equal";
}

TEST(io_json, precision_comparison)
{
    namespace fs = std::filesystem;
    using json = nlohmann::ordered_json;

    // Build paths
    const fs::path project_root(PROJECT_DIR);
    const fs::path output_path = project_root / "precision_comparison_test.json";

    // Make simulation data
    SimulationData sd_original;
    SimulationParameters& simpar_original = sd_original.get_simulation_parameters();
    simpar_original.latitude = SolTrace::Data::PI / 2.;
    simpar_original.longitude = 1.5816981651658435135814384351384351385143845;
    simpar_original.tolerance = 1e-90;
    simpar_original.seed = 15654681468168136541;

    // Add sun
    auto sun = make_ray_source<Sun>();
    double nan = std::numeric_limits<double>::quiet_NaN();
    sun->set_shape(SolTrace::Data::SunShape::GAUSSIAN, 1.0, nan, nan, {}, {});
    sd_original.add_ray_source(sun);

    // Export to JSON
    ASSERT_NO_THROW(sd_original.export_json_file(output_path.string())) << "Failed to export first JSON";

    // Import from first JSON
    SimulationData sd_round_trip;
    ASSERT_NO_THROW(sd_round_trip.import_json_file(output_path.string())) << "Failed to import JSON";

    // Compare sim parameters
    SimulationParameters& simpar_round_trip = sd_round_trip.get_simulation_parameters();
    EXPECT_DOUBLE_EQ(simpar_original.latitude, simpar_round_trip.latitude);
    EXPECT_DOUBLE_EQ(simpar_original.longitude, simpar_round_trip.longitude);
    EXPECT_DOUBLE_EQ(simpar_original.tolerance, simpar_round_trip.tolerance);
    EXPECT_EQ(simpar_original.seed, simpar_round_trip.seed);

    // Compare sun nan parameters
    double half_width_original = sd_original.get_ray_source()->get_half_width();
    double half_width_round_trip = sd_round_trip.get_ray_source()->get_half_width();
    double csr_original = sd_original.get_ray_source()->get_circumsolar_ratio();
    double csr_round_trip = sd_round_trip.get_ray_source()->get_circumsolar_ratio();

    EXPECT_TRUE(std::isnan(half_width_original) && std::isnan(half_width_round_trip));
    EXPECT_TRUE(std::isnan(csr_original) && std::isnan(csr_round_trip));
}

TEST(io_json, empty_case)
{
    namespace fs = std::filesystem;
    using json = nlohmann::ordered_json;

    // Build paths
    const fs::path project_root(PROJECT_DIR);
    const fs::path output_path = project_root / "empty_case.json";

    // Make simulation data
    SimulationData sd_original;

    // Export
    ASSERT_NO_THROW(sd_original.export_json_file(output_path.string())) << "Failed to export first JSON";

    // Import from first JSON
    SimulationData sd_round_trip;
    ASSERT_NO_THROW(sd_round_trip.import_json_file(output_path.string())) << "Failed to import JSON";

    // Check number of elements
    EXPECT_TRUE(sd_round_trip.get_number_of_elements() == 0);
    EXPECT_TRUE(sd_round_trip.get_number_of_ray_sources() == 0);
}

TEST(io_json, invalid_sun_shape)
{
    SimulationData sd;
    auto sun = SolTrace::Data::make_ray_source<Sun>();
    sun->set_shape(SunShape::GAUSSIAN, 0.01, 0.0, 0.0);
    sd.add_ray_source(sun);
    const std::string path = "invalid_sun_shape.json";
    sd.export_json_file(path);

    // Tamper JSON
    nlohmann::ordered_json root;
    {
        std::ifstream ifs(path);
        ifs >> root;
    }
    root["ray_sources"]["0"]["my_shape"] = "Ellipse"; // invalid
    {
        std::ofstream ofs(path, std::ios::trunc);
        ofs << root.dump(2);
    }

    SimulationData sd2;
    EXPECT_THROW(sd2.import_json_file(path), std::runtime_error);
}

TEST(io_json, invalid_source_type)
{
    SimulationData sd;
    auto sun = SolTrace::Data::make_ray_source<Sun>();
    sun->set_shape(SunShape::GAUSSIAN, 0.01, 0.0, 0.0);
    sd.add_ray_source(sun);
    const std::string path = "invalid_source_type.json";
    sd.export_json_file(path);

    // Tamper JSON
    nlohmann::ordered_json root;
    {
        std::ifstream ifs(path);
        ifs >> root;
    }
    root["ray_sources"]["0"]["source_type"] = "The Moon"; // invalid
    {
        std::ofstream ofs(path, std::ios::trunc);
        ofs << root.dump(2);
    }

    SimulationData sd2;
    EXPECT_THROW(sd2.import_json_file(path), std::runtime_error);
}

TEST(io_json, multi_ray_source)
{
    namespace fs = std::filesystem;
    using json = nlohmann::ordered_json;

    // Build paths
    const fs::path project_root(PROJECT_DIR);
    const fs::path output_path = project_root / "multi_ray_source.json";

    // Make simulation data
    SimulationData sd_original;

    // Add sun
    auto sun = make_ray_source<Sun>();
    double nan = std::numeric_limits<double>::quiet_NaN();
    sun->set_shape(SolTrace::Data::SunShape::GAUSSIAN, 1.0, nan, nan, {}, {});
    sd_original.add_ray_source(sun);

    auto sun2 = make_ray_source<Sun>();
    sun2->set_shape(SolTrace::Data::SunShape::GAUSSIAN, 1.0, nan, nan, {}, {});
    sd_original.add_ray_source(sun2);

    // Export to JSON
    ASSERT_NO_THROW(sd_original.export_json_file(output_path.string())) << "Failed to export first JSON";

    // Import from first JSON
    SimulationData sd_round_trip;
    ASSERT_NO_THROW(sd_round_trip.import_json_file(output_path.string())) << "Failed to import JSON";

    // Check ray sources
    EXPECT_EQ(sd_original.get_number_of_ray_sources(), sd_round_trip.get_number_of_ray_sources());
    EXPECT_EQ(sd_round_trip.get_number_of_ray_sources(), 2);
}

TEST(io_json, performance_comparison)
{
    namespace fs = std::filesystem;
    using json = nlohmann::ordered_json;
    using SolTrace::Runner::RunnerStatus;
    using SolTrace::NativeRunner::NativeRunner;
    using SolTrace::NativeRunner::TRayData;
    using SolTrace::NativeRunner::tstage_ptr;
    using SolTrace::NativeRunner::TSystem;

    // Build paths
    const fs::path project_root(PROJECT_DIR);
    const fs::path sample_path = project_root / "High Flux Solar Furnace.stinput";
    const fs::path output_path_1 = project_root / "json_test_1.json";
    const fs::path output_path_2 = project_root / "json_test_2.json";

    ASSERT_TRUE(fs::exists(sample_path)) << "Sample .stinput not found: " << sample_path.string();

    // Load original simulation from .stinput
    SimulationData sd_original;
    ASSERT_TRUE(sd_original.import_from_file(sample_path.string())) << "Failed to import stinput";

    // Export to JSON
    ASSERT_NO_THROW(sd_original.export_json_file(output_path_1.string())) << "Failed to export first JSON";

    // Import from first JSON
    SimulationData sd_round_trip;
    ASSERT_NO_THROW(sd_round_trip.import_json_file(output_path_1.string())) << "Failed to import JSON";

    // Run original case
    NativeRunner runner;
    RunnerStatus sts;
    sts = runner.initialize();
    ASSERT_EQ(sts, RunnerStatus::SUCCESS) << "runner.initialize() failed";
    sts = runner.setup_simulation(&sd_original);
    ASSERT_EQ(sts, RunnerStatus::SUCCESS) << "runner.setup_simulation() failed";
    sts = runner.run_simulation();
    ASSERT_EQ(sts, RunnerStatus::SUCCESS) << "runner.run_simulation() failed";
    SimulationResult result_original;
    sts = runner.report_simulation(&result_original, 0);
    ASSERT_EQ(sts, RunnerStatus::SUCCESS) << "runner.report_simulation() failed";

    // Run round trip case
    NativeRunner runner_round_trip;
    sts = runner_round_trip.initialize();
    ASSERT_EQ(sts, RunnerStatus::SUCCESS) << "runner_round_trip.initialize() failed";
    sts = runner_round_trip.setup_simulation(&sd_round_trip);
    ASSERT_EQ(sts, RunnerStatus::SUCCESS) << "runner_round_trip.setup_simulation() failed";
    sts = runner_round_trip.run_simulation();
    ASSERT_EQ(sts, RunnerStatus::SUCCESS) << "runner_round_trip.run_simulation() failed";
    SimulationResult result_round_trip;
    sts = runner_round_trip.report_simulation(&result_round_trip, 0);
    ASSERT_EQ(sts, RunnerStatus::SUCCESS) << "runner_round_trip.report_simulation() failed";
    
    // Compare number of records
    ASSERT_EQ(result_original.get_number_of_records(), result_round_trip.get_number_of_records());

    // Loop through each result set and compare
    for (int i = 0; i < result_original.get_number_of_records(); ++i)
    {
        ray_record_ptr rr_o = result_original[i];
        ray_record_ptr rr_r = result_round_trip[i];

        ASSERT_EQ(rr_o->get_number_of_interactions(),
            rr_r->get_number_of_interactions());

        for (int_fast64_t k = 0; k < rr_o->get_number_of_interactions(); ++k)
        {
            // Event & element IDs
            ASSERT_EQ(rr_o->get_event(k), rr_r->get_event(k));
            ASSERT_EQ(rr_o->get_element(k), rr_r->get_element(k));

            // Positions
            Vector3d pos_o; Vector3d pos_r;
            rr_o->get_position(k, pos_o);
            rr_r->get_position(k, pos_r);
            EXPECT_DOUBLE_EQ(pos_o[0], pos_r[0]);
            EXPECT_DOUBLE_EQ(pos_o[1], pos_r[1]);
            EXPECT_DOUBLE_EQ(pos_o[2], pos_r[2]);

            // Directions
            Vector3d dir_o; Vector3d dir_r;
            rr_o->get_direction(k, dir_o);
            rr_r->get_direction(k, dir_r);
            EXPECT_DOUBLE_EQ(dir_o[0], dir_r[0]);
            EXPECT_DOUBLE_EQ(dir_o[1], dir_r[1]);
            EXPECT_DOUBLE_EQ(dir_o[2], dir_r[2]);
        }

    }
    

}