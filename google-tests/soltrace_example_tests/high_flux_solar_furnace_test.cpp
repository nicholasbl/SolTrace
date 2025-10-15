#include <gtest/gtest.h>

#include "strace.h"
#include "stapi.h"
#include "split_csv.h"

#include <fstream>

using namespace std;

TEST(HighFluxSolarFurnace, RaysTraced1e5OpticalErrorsOff)
{
	// Pulling in path variable from CMake and creating path to .stinput sample file
	string sample_path = string(PROJECT_DIR) + string("/High Flux Solar Furnace.stinput");

	// Path to .csv exported from Soltrace as ground truth
	string ground_csv_path = PROJECT_DIR + string("/hfsf_example_raydata.csv");

	std::ifstream csv_file(ground_csv_path);
	vector<vector<string>> ground_raydata = split_csv(ground_csv_path);

	// Soltrace case parameters
	const char* file = sample_path.data();
	int nrays = 10000;
	int maxrays = 1000000;
	int seed = 1; // Any positive integer will produce the same results each time, -1 will be a random seed
	int sunshape = 0;
	int errors = 0;
	int powertower = 0; // Toggles optimizations for power tower cases

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

	::st_sim_params(cxt, nrays, maxrays, powertower);
	::st_sim_errors(cxt, sunshape, errors);
	code = ::st_sim_run_with_refactor(cxt, (unsigned int)seed, ::trace_progress, 0, true);

	int Length = ::st_num_intersections(cxt);

	double* x_location = new double[Length];
	double* y_location = new double[Length];
	double* z_location = new double[Length];

	double* x_cos = new double[Length];
	double* y_cos = new double[Length];
	double* z_cos = new double[Length];

	int* ray_numbers = new int[Length];
	int* element_map = new int[Length];
	int* stage_map = new int[Length];

	// Retrieving data from Soltrace simulation

	::st_locations(cxt, x_location, y_location, z_location);
	::st_cosines(cxt, x_cos, y_cos, z_cos);
	::st_raynumbers(cxt, ray_numbers);
	::st_elementmap(cxt, element_map);
	::st_stagemap(cxt, stage_map);

	for (size_t i = 0; i < Length; i++)
	{
		EXPECT_NEAR(x_location[i], stod(ground_raydata[0][i + 1]), 0.01);
		EXPECT_NEAR(y_location[i], stod(ground_raydata[1][i + 1]), 0.01);
		EXPECT_NEAR(z_location[i], stod(ground_raydata[2][i + 1]), 0.01);

		EXPECT_NEAR(x_cos[i], stod(ground_raydata[3][i + 1]), 0.01);
		EXPECT_NEAR(y_cos[i], stod(ground_raydata[4][i + 1]), 0.01);
		EXPECT_NEAR(z_cos[i], stod(ground_raydata[5][i + 1]), 0.01);

		EXPECT_EQ(element_map[i], stod(ground_raydata[6][i + 1]));
		EXPECT_EQ(stage_map[i], stod(ground_raydata[7][i + 1]));
		EXPECT_EQ(ray_numbers[i], stod(ground_raydata[8][i + 1]));

	}

	delete[] x_location;
	delete[] y_location;
	delete[] z_location;

	delete[] x_cos;
	delete[] y_cos;
	delete[] z_cos;

	delete[] element_map;
	delete[] stage_map;
	delete[] ray_numbers;



}