#include <gtest/gtest.h>
#include <windows.h> 

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
	char buffer[MAX_PATH];
	DWORD length = GetCurrentDirectoryA(MAX_PATH, buffer);
	if (length == 0) {
		std::cerr << "Error getting current directory!" << std::endl;
	}
	else {
		std::cout << "Current working directory: " << buffer << std::endl;
	}

	const char* additional_path = "../Aperture Examples.stinput";

	if (length + strlen(additional_path) < MAX_PATH) {
		strcat(buffer, additional_path); // Append the additional path to the current directory
		std::cout << "New path: " << buffer << std::endl;
	}
	else {
		std::cerr << "Buffer too small to append!" << std::endl;
	}



	const char* file = additional_path;
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


	double* x_round = new double[Length];
	double* y_round = new double[Length];
	double* z_round = new double[Length];

	double* x_cos_round = new double[Length];
	double* y_cos_round = new double[Length];
	double* z_cos_round = new double[Length];

	for (int i = 0; i != Length; i++)
	{
		x_round[i] = roundToDecimalPlaces(x_location[i], 4);
		y_round[i] = roundToDecimalPlaces(y_location[i], 4);
		z_round[i] = roundToDecimalPlaces(z_location[i], 4);

		x_cos_round[i] = roundToDecimalPlaces(x_cos[i], 4);
		y_cos_round[i] = roundToDecimalPlaces(y_cos[i], 4);
		z_cos_round[i] = roundToDecimalPlaces(z_cos[i], 4);
	}
	

	for (int i = 0; i != Length; i++)
	{
		EXPECT_EQ(ray_data[i], 1);
		EXPECT_EQ(stage_map[i], 1);
	}
	cerr << "Ray numbers and stage numbers match." << endl;

	EXPECT_EQ(element_map[0], 2);
	EXPECT_EQ(element_map[1], 1);
	EXPECT_EQ(element_map[2], 2);
	cerr << "Elements each ray hits match." << endl;

	EXPECT_EQ(x_round[0], 11.57);
	EXPECT_EQ(x_round[1], 0.5502);
	EXPECT_EQ(x_round[2], -12.6424);


	EXPECT_EQ(y_round[0], -4.534);
	EXPECT_EQ(y_round[1], -0.2156);
	EXPECT_EQ(y_round[2], 4.9542);

	EXPECT_EQ(z_round[0], 1.3163);
	EXPECT_EQ(z_round[1], 14.9843);
	EXPECT_EQ(z_round[2], 1.7656);
	cerr << "All intersection positions match." << endl;

	EXPECT_EQ(x_cos_round[0], 0);
	EXPECT_EQ(x_cos_round[1], -0.6095);
	EXPECT_EQ(x_cos_round[2], -0.6808);

	EXPECT_EQ(y_cos_round[0], 0);
	EXPECT_EQ(y_cos_round[1], 0.2388);
	EXPECT_EQ(y_cos_round[2], 0.2668);

	EXPECT_EQ(z_cos_round[0], -1);
	EXPECT_EQ(z_cos_round[1], 0.756);
	EXPECT_EQ(z_cos_round[2], -0.6822);
	cerr << "All intersection cosines match." << endl;

	EXPECT_EQ(Length, 3);
}

