
#include <gtest/gtest.h>

#include <aperture.hpp>
#include <constants.hpp>

#include "common.hpp"


TEST(io_json, json_write)
{
	// Pulling in path variable from CMake and creating path to .stinput sample file
	std::string sample_path = std::string(PROJECT_DIR) + std::string("/High Flux Solar Furnace.stinput");
	std::string output_path = std::string(PROJECT_DIR) + std::string("/json_test.json");

	// Create Simuluation Data
	SimulationData sd;
	sd.import_from_file(sample_path);

	// Write Simulation Data
	sd.export_json_file(output_path);

	int break_here = 0;


}