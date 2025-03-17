#include <gtest/gtest.h>

#include <cmath>

#include "strace.h"
#include "stapi.h"

using namespace std;

double roundToDecimalPlaces(double value, int decimalPlaces) {
	const double multiplier = std::pow(10.0, decimalPlaces);
	return std::round(value * multiplier) / multiplier;
}

TEST(ApertureExamples, SingleRayNoOpticalErrors)
{
	// Pulling in path variable from CMake and creating path to .stinput sample file
	string sample_path = string(PROJECT_DIR) + string("/Aperture Examples.stinput");
	
	// Path to .csv exported from Soltrace as ground truth
	const char* file = sample_path.data();

	// Soltrace case parameters
	int nrays = 1;
	int maxrays = 50;
	int seed = 1; // Any positive integer will produce the same results each time, -1 will be a random seed
	int sunshape = 0;
	int errors = 0;
	int aspointfocus = 0; // Toggles optimizations for power tower cases

	int code = 0;

	// Creates system context for Soltrace cases
	st_context_t cxt = ::st_create_context();

	FILE* fp = fopen(file, "r");
	if (!fp)
	{
		printf("failed to open system input file\n");
		code = -1;
	}

	printf("input file: %s\n", file);
	if (!read_system(fp, cxt))
	{
		printf("error in input file.\n");
		fclose(fp);
		code = -1;
	}

	fclose(fp);

	::st_sim_params(cxt, nrays, maxrays, aspointfocus);
	::st_sim_errors(cxt, sunshape, errors);
	code = ::st_sim_run(cxt, (unsigned int)seed, ::trace_progress, 0);

	int Length = ::st_num_intersections(cxt);
	
	double* x_location = new double[Length];
	double* y_location = new double[Length];
	double* z_location = new double[Length];

	double* x_cos = new double[Length];
	double* y_cos = new double[Length];
	double* z_cos = new double[Length];

	int* ray_data = new int[Length];
	int* element_map = new int[Length];
	int* stage_map = new int[Length];

	// Retrieving data from Soltrace simulation
	::st_locations(cxt, x_location, y_location, z_location);
	::st_cosines(cxt, x_cos, y_cos, z_cos);
	::st_raynumbers(cxt, ray_data);
	::st_elementmap(cxt, element_map);
	::st_stagemap(cxt, stage_map);
	

	for (int i = 0; i != Length; i++)
	{
		EXPECT_EQ(ray_data[i], 1);
		EXPECT_EQ(stage_map[i], 1);
	}

	EXPECT_EQ(element_map[0], 1);
	

	EXPECT_NEAR(x_location[0], -3.06214, 0.0001);
	EXPECT_NEAR(y_location[0], 5.92862, 0.0001);
	EXPECT_NEAR(z_location[0], 12.7732, 0.0001);

	EXPECT_NEAR(x_cos[0], 1.22465e-16, 0.0000000000000001);
	EXPECT_NEAR(y_cos[0], 0, 0.0001);
	EXPECT_NEAR(z_cos[0], -1, 0.0001);

	EXPECT_EQ(Length, 1);
}

