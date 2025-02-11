#include <gtest/gtest.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>

#include <string>
#include <cstring>
#include <vector>
#include <exception>
#include <cstdlib>

#include "strace.h"
#include "stapi.h"

using namespace std;

double roundToDecimalPlaces(double value, int decimalPlaces) {
	const double multiplier = std::pow(10.0, decimalPlaces);
	return std::round(value * multiplier) / multiplier;
}

TEST(st_sim_run_test, BasicAssertions)
{
	cout << "Project Directory: " << PROJECT_DIR << endl;
	string project_dir = PROJECT_DIR;
	//if (length == 0) {
	//	std::cerr << "Error getting current directory!" << std::endl;
	//}
	//else {
	//	std::cout << "Current working directory: " << buffer << std::endl;
	//}
	cout << "Current working directory: " << project_dir << endl;

	string additional_path = "/Aperture Examples.stinput";
	project_dir += additional_path;

	cout << "File path: " << project_dir << endl;


	const char* file = project_dir.data();
	int nrays = 1;
	int maxrays = 50;
	int seed = 1;
	int sunshape = 0;
	int errors = 0;
	int aspointfocus = 0;

	int code = 0;

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

	// Retrieving ray intersection locations
	::st_locations(cxt, x_location, y_location, z_location);

	// Retrieving cosines of ray intersections
	::st_cosines(cxt, x_cos, y_cos, z_cos);

	// Retrieving ray numbers for each ray
	::st_raynumbers(cxt, ray_data);

	// Retrieving element of simulation that each ray hit
	::st_elementmap(cxt, element_map);

	// Retrieving stage each ray is in during simulation
	::st_stagemap(cxt, stage_map);
	

	for (int i = 0; i != Length; i++)
	{
		EXPECT_EQ(ray_data[i], 1);
		EXPECT_EQ(stage_map[i], 1);
	}
	cerr << "Ray numbers and stage numbers match." << endl;

	EXPECT_EQ(element_map[0], 1);
	
	cerr << "Elements each ray hits match." << endl;

	EXPECT_NEAR(x_location[0], -3.06214, 0.0001);
	EXPECT_NEAR(y_location[0], 5.92862, 0.0001);
	EXPECT_NEAR(z_location[0], 12.7732, 0.0001);
	cerr << "All intersection positions match." << endl;

	EXPECT_NEAR(x_cos[0], 1.22465e-16, 0.0000000000000001);
	EXPECT_NEAR(y_cos[0], 0, 0.0001);
	EXPECT_NEAR(z_cos[0], -1, 0.0001);
	cerr << "All intersection cosines match." << endl;

	EXPECT_EQ(Length, 1);
}

