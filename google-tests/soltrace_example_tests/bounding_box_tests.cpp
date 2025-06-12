#include <gtest/gtest.h>

#include <cmath>

#include "strace.h"
#include "stapi.h"
#include "split_csv.h"
#include "types.h"
#include "procs.h"
#include "bbox_calculator.h"

using namespace std;

void expand_box(double min, double max, double f,
	double& min_expanded, double& max_expanded)
{
	double dist = max - min;
	double dist_expanded = dist * f;
	double diff = dist_expanded - dist;
	min_expanded = min - 0.5 * diff;
	max_expanded = max + 0.5 * diff;
}

void irradiate_single_axis(TElement* element, const double(&min_bbox_coord)[3], const double(&max_bbox_coord)[3],
	const double(&min_gen_coord)[3], const double(&max_gen_coord)[3],
	const int i, const int j, const int k, const int N_grid, 
	bool& is_element_contained, double& bbox_hitrate)
{
	// Get stage
	TStage* stage = element->parent_stage;

	// i is index of axis rays are pointing in
	// j and k make plane rays are positioned at

	double j_dist = max_gen_coord[j] - min_gen_coord[j];
	double j_stepsize = j_dist / (double)(N_grid - 1);
	double j_dir = 0.f;

	double k_dist = max_gen_coord[k] - min_gen_coord[k];
	double k_stepsize = k_dist / (double)(N_grid - 1);
	double k_dir = 0.f;

	double i_dist = max_gen_coord[i] - min_gen_coord[i];
	double i_pos = max_gen_coord[i] + 10 * i_dist;
	double i_dir = -1.f;

	// Initialize results
	is_element_contained = true;
	int NRays = 0;
	int NHit = 0;

	// Loop through j axis
	for (int j_step = 0; j_step < N_grid; j_step++) 
	{
		// Loop through k axis
		for (int k_step = 0; k_step < N_grid; k_step++)
		{
			// Calculate ray position and direction
			double j_pos = min_gen_coord[j] + (j_stepsize * j_step);
			double k_pos = min_gen_coord[k] + (k_stepsize * k_step);
			
			double PosRayGlob[3];
			double CosRayGlob[3];

			PosRayGlob[i] = i_pos; PosRayGlob[j] = j_pos; PosRayGlob[k] = k_pos;
			CosRayGlob[i] = i_dir; CosRayGlob[j] = j_dir; CosRayGlob[k] = k_dir;

			// Transform to element coordinate system
			double PosRayStage[3], CosRayStage[3];
			::TransformToLocal(PosRayGlob, CosRayGlob,
				stage->Origin, stage->RRefToLoc,
				PosRayStage, CosRayStage);
			double PosRayElement[3], CosRayElement[3];
			::TransformToLocal(PosRayStage, CosRayStage,
				element->Origin, element->RRefToLoc,
				PosRayElement, CosRayElement);

			// Check intersection
			double PosRaySurfElement[3], CosRaySurfElement[3], DFXYZ[3];
			double PathLength;
			int ErrorFlag, InterceptFlag, HitBackSide;
			::DetermineElementIntersectionNew(element, PosRayElement, CosRayElement,
				PosRaySurfElement, CosRaySurfElement, DFXYZ,
				&PathLength, &ErrorFlag, &InterceptFlag, &HitBackSide);

			// Check if Element Z location is within ZAperture
			if (PosRaySurfElement[2] > element->ZAperture)
			{
				if ((std::tolower(element->SurfaceIndex) != 'm')
					|| (std::tolower(element->SurfaceIndex) != 'r'))
				{
					// Does NOT actually hit
					InterceptFlag = 0;
				}
				
			}

			// Process results
			NRays++;
			if (InterceptFlag == 1)
			{
				NHit++;

				// Transform intersection location back to global
				double PosRaySurfStage[3], CosRaySurfStage[3],
					PosRaySurfGlob[3], CosRaySurfGlob[3];
				TransformToReference(PosRaySurfElement, CosRaySurfElement,
					element->Origin, element->RLocToRef,
					PosRaySurfStage, CosRaySurfStage);
				TransformToReference(PosRaySurfStage, CosRaySurfStage,
					stage->Origin, stage->RLocToRef,
					PosRaySurfGlob, CosRaySurfGlob);

				// Check if hit location is within bounding box
				for (int axis = 0; axis < 3; axis++)
				{
					double pos = PosRaySurfGlob[axis];
					if (pos < min_bbox_coord[axis] || pos > max_bbox_coord[axis]) 
					{
						is_element_contained = false;
						return;
					}
				}
			}
		}
	}

	// Element is within bbox
	// Calculate hit rate
	double area_bbox = (max_bbox_coord[j] - min_bbox_coord[j])
		* (max_bbox_coord[k] - min_bbox_coord[k]);
	double area_gen = j_dist * k_dist;

	double gen_hitrate = (double)NHit / (double)NRays;
	bbox_hitrate = gen_hitrate * (area_bbox / area_gen);
}

void bbox_test_single_element(TElement* element, double gen_size_factor, int N_grid)
{
	// Generate bounding box
	float min_bbox_coord_f[3];
	float max_bbox_coord_f[3];
	bbox_calculator::BBOXERRORS error = bbox_calculator::get_bounds(element, min_bbox_coord_f, max_bbox_coord_f);

	// Convert bounding box coord to double
	double min_bbox_coord[3], max_bbox_coord[3];
	for (int i = 0; i < 3; ++i) {
		min_bbox_coord[i] = static_cast<double>(min_bbox_coord_f[i]);
		max_bbox_coord[i] = static_cast<double>(max_bbox_coord_f[i]);
	}

	// Check for bounding box errors
	if (error != bbox_calculator::NONE)
		FAIL() << "Bounding box calculation error";

	// Make ray generation range
	double min_gen_coord[3], max_gen_coord[3];
	for (int i = 0; i < 3; i++)
	{
		expand_box(min_bbox_coord[i], max_bbox_coord[i], gen_size_factor,
			min_gen_coord[i], max_gen_coord[i]);
	}

	// Cycle through grid, sending rays for each plane
	// [x, y, z] -> [yz, xz, xy]
	int axes[3] = { 0, 1, 2 };
	bool is_element_hit = false;
	double bbox_hitrates[3] = { 0,0,0 };
	for (int i = 0; i < 3; i++)
	{
		// i represents the direction the rays are aiming, 
		// so the plane the rays are coming from are the other two axes
		int j = axes[(i + 1) % 3];
		int k = axes[(i + 2) % 3];
		bool is_element_contained;
		irradiate_single_axis(element, min_bbox_coord, max_bbox_coord, min_gen_coord, max_gen_coord,
			i, j, k, N_grid, is_element_contained, bbox_hitrates[i]);

		if (is_element_contained == false)
		{
			FAIL() << "Element not contained in bounding box";
			return;
		}

		if (bbox_hitrates[i] > 0.0)
			is_element_hit = true;
	}

	// Was element ever hit?
	if (is_element_hit == false)
	{
		FAIL() << "Element not hit by any rays";
		return;
	}

	// SUCCESS
	SUCCEED() << "Aperture: " << element->ShapeIndex 
		<< "Surface: " << element->SurfaceIndex 
		<< "bbox_hitrates: ["
		<< bbox_hitrates[0] << ", "
		<< bbox_hitrates[1] << ", "
		<< bbox_hitrates[2] << "]";

	return;
}


// Get geometries from file
void file_bbox_test(string sample_path)
{
	// Soltrace case parameters
	const char* file = sample_path.data();
	int code = 0;

	// Creates system context for Soltrace cases
	st_context_t cxt = ::st_create_context();

	FILE* fp = fopen(file, "r");
	if (!fp)
	{
		printf("failed to open system input file\n");
		code = -1;
		FAIL() << "failed to open system input file";
		return;
	}

	printf("input file: %s\n", file);
	if (!read_system(fp, cxt, string(PROJECT_DIR).c_str()))
	{
		printf("error in input file.\n");
		fclose(fp);
		code = -1;
		FAIL() << "error in input file";
		return;
	}

	fclose(fp);

	TSystem* sys = reinterpret_cast<TSystem*>(cxt);
	sys->AllRayData.Clear();

	if (!InitGeometries(sys))
	{
		code = -1;
		FAIL() << "failed to init geometries";
	}

	double gen_size_factor = 1.5;
	int N_grid = 100;

	// Loop through stages
	for (st_uint_t i = 0; i < sys->StageList.size(); i++)
	{
		TStage* stage = sys->StageList[i];

		// Loop through elements
		for (st_uint_t j = 0; j < stage->ElementList.size(); j++)
		{
			// Assign parent stage (for embree purposes)
			TElement* element = stage->ElementList[j];
			element->parent_stage = stage;

			// Test element
			bbox_test_single_element(element, gen_size_factor, N_grid);
		}
	}


}

TEST(BoundingBoxTests, ApertureExamples)
{
	// Pulling in path variable from CMake and creating path to .stinput sample file
	string sample_path = string(PROJECT_DIR) + string("/Aperture Examples.stinput");

	// Compare runs with and without embree to saved raydata
	file_bbox_test(sample_path);
}

TEST(BoundingBoxTests, ApertureSampleBuiltIn)
{
	// Pulling in path variable from CMake and creating path to .stinput sample file
	string sample_path = string(PROJECT_DIR) + string("/Aperture Sample-Built In.stinput");

	// Compare runs with and without embree to saved raydata
	file_bbox_test(sample_path);
}

TEST(BoundingBoxTests, ApertureSampleExternal)
{
	// Pulling in path variable from CMake and creating path to .stinput sample file
	string sample_path = string(PROJECT_DIR) + string("/Aperture Sample-External Files.stinput");

	// Compare runs with and without embree to saved raydata
	file_bbox_test(sample_path);
}
