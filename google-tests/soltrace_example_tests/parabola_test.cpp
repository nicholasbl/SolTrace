#include <gtest/gtest.h>

#include "strace.h"
#include "stapi.h"
#include "split_csv.h"

using namespace std;

// This test does not seem to work, location values are different from ground values 
// taken from Soltrace executable. The parabola.mon file appears to be being loaded correctly.

// Should test with fewer rays and check values more closely, see if cos values differ
TEST(st_parabola_sim_run_test, BasicAssertions)
{
	cout << "Project Directory: " << PROJECT_DIR << endl;
	string project_dir = PROJECT_DIR;
	
	cout << "Current working directory: " << project_dir << endl;

	//parabola.mon is loaded through input.cpp in coretrace. To work in Visual Studio debug,
	//must be copied to debug directory in google-tests
	string file_path = "/parabola.stinput";
	string csv_path = "/parabola_example_raydata.csv";

	string input_file_path = project_dir + file_path;
	string ground_csv_path = project_dir + csv_path;

	cout << "File path: " << project_dir << endl;

	std::ifstream csv_file(ground_csv_path);
	vector<vector<string>> result = split_csv(ground_csv_path);


	const char* sample_file_path = input_file_path.data();
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

	// Retrieving ray intersection locations
	::st_locations(cxt, x_location, y_location, z_location);

	// Retrieving cosines of ray intersections
	::st_cosines(cxt, x_cos, y_cos, z_cos);

	// Retrieving ray numbers for each ray
	::st_raynumbers(cxt, ray_numbers);

	// Retrieving element of simulation that each ray hit
	::st_elementmap(cxt, element_map);

	// Retrieving stage each ray is in during simulation
	::st_stagemap(cxt, stage_map);

	for (size_t i = 0; i < Length; i++)
	{
		ASSERT_NEAR(x_location[i], stod(result[0][i + 1]), 0.01);
		ASSERT_NEAR(y_location[i], stod(result[1][i + 1]), 0.01);
		ASSERT_NEAR(z_location[i], stod(result[2][i + 1]), 0.01);

		EXPECT_NEAR(x_cos[i], stod(result[3][i + 1]), 0.01);
		EXPECT_NEAR(y_cos[i], stod(result[4][i + 1]), 0.01);
		EXPECT_NEAR(z_cos[i], stod(result[5][i + 1]), 0.01);

		EXPECT_NEAR(element_map[i], stod(result[6][i + 1]), 0.01);
		EXPECT_EQ(stage_map[i], stod(result[7][i + 1]));
		EXPECT_EQ(ray_numbers[i], stod(result[8][i + 1]));

	}

	cerr << "All intersection positions match." << endl;
	cerr << "Ray numbers and stage numbers match." << endl;
	cerr << "Elements each ray hits match." << endl;
	cerr << "All intersection cosines match." << endl;

}