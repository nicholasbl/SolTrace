
#include <gtest/gtest.h>

#include <aperture.hpp>
#include <constants.hpp>

#include "common.hpp"
TEST(io_json, json_write)
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