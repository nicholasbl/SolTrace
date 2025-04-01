#include <gtest/gtest.h>

#include "strace.h"
#include "stapi.h"
#include "split_csv.h"

using namespace std;


TEST(st_parabola_sim_run_test, BasicAssertions)
{
	string sample_path = string(PROJECT_DIR) + string("/parabola.stinput");

	//parabola.mon is loaded through input.cpp in coretrace. To work in Visual Studio debug,
	//must be copied to debug directory in google-tests
	string ground_csv_path = PROJECT_DIR + string("/parabola_example_raydata.csv");

	std::ifstream csv_file(ground_csv_path);
	vector<vector<string>> ground_raydata = split_csv(ground_csv_path);


	const char* sample_file_path = sample_path.data();
	int nrays = 10000;
	int maxrays = 100000;
	int seed = 1;
	int sunshape = 0;
	int errors = 0;
	int powertower = 0;

	int code = 0;

	st_context_t cxt = ::st_create_context();

	FILE* sample_file = fopen(sample_file_path, "r");
	if (!sample_file)
	{
		printf("failed to open system input file\n");
		code = -1;
	}

	printf("input file: %s\n", sample_file);
	if (!read_system(sample_file, cxt))
	{
		printf("error in input file.\n");
		fclose(sample_file);
		code = -1;
	}

	fclose(sample_file);

	::st_sim_params(cxt, nrays, maxrays, powertower);
	::st_sim_errors(cxt, sunshape, errors);
	code = ::st_sim_run(cxt, (unsigned int)seed, ::trace_progress, 0);

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
		ASSERT_NEAR(x_location[i], stod(ground_raydata[0][i + 1]), 0.01);
		ASSERT_NEAR(y_location[i], stod(ground_raydata[1][i + 1]), 0.01);
		ASSERT_NEAR(z_location[i], stod(ground_raydata[2][i + 1]), 0.01);

		EXPECT_NEAR(x_cos[i], stod(ground_raydata[3][i + 1]), 0.01);
		EXPECT_NEAR(y_cos[i], stod(ground_raydata[4][i + 1]), 0.01);
		EXPECT_NEAR(z_cos[i], stod(ground_raydata[5][i + 1]), 0.01);

		EXPECT_NEAR(element_map[i], stod(ground_raydata[6][i + 1]), 0.01);
		EXPECT_EQ(stage_map[i], stod(ground_raydata[7][i + 1]));
		EXPECT_EQ(ray_numbers[i], stod(ground_raydata[8][i + 1]));

	}

	cerr << "All intersection positions match." << endl;
	cerr << "Ray numbers and stage numbers match." << endl;
	cerr << "Elements each ray hits match." << endl;
	cerr << "All intersection cosines match." << endl;

}