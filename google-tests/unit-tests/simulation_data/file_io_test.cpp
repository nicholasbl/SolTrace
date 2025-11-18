
#include <gtest/gtest.h>

#include <aperture.hpp>
#include <constants.hpp>

#include "common.hpp"


TEST(io_json, json_write)
{
	// Pulling in path variable from CMake and creating path to .stinput sample file
	std::string sample_path = std::string(PROJECT_DIR) + std::string("/High Flux Solar Furnace.stinput");
	std::string output_path = std::string(PROJECT_DIR) + std::string("/json_test.json");
	std::string output_path_2 = std::string(PROJECT_DIR) + std::string("/json_test_2.json");

	// Create simuluation data (load stinput sample case)
	SimulationData sd_exported;
	sd_exported.import_from_file(sample_path);

	// Write simulation data
	sd_exported.export_json_file(output_path);

	// Read simulation data
	SimulationData sd_imported;
	sd_imported.import_json_file(output_path);

	// Write second simulation data
	sd_imported.export_json_file(output_path_2);

	int break_here = 0;


}