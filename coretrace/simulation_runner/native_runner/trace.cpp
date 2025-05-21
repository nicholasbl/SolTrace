
/*******************************************************************************************************
 *  Copyright 2018 Alliance for Sustainable Energy, LLC
 *
 *  NOTICE: This software was developed at least in part by Alliance for Sustainable Energy, LLC
 *  ("Alliance") under Contract No. DE-AC36-08GO28308 with the U.S. Department of Energy and the U.S.
 *  The Government retains for itself and others acting on its behalf a nonexclusive, paid-up,
 *  irrevocable worldwide license in the software to reproduce, prepare derivative works, distribute
 *  copies to the public, perform publicly and display publicly, and to permit others to do so.
 *
 *  Redistribution and use in source and binary forms, with or without modification, are permitted
 *  provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice, the above government
 *  rights notice, this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright notice, the above government
 *  rights notice, this list of conditions and the following disclaimer in the documentation and/or
 *  other materials provided with the distribution.
 *
 *  3. The entire corresponding source code of any redistribution, with or without modification, by a
 *  research entity, including but not limited to any contracting manager/operator of a United States
 *  National Laboratory, any institution of higher learning, and any non-profit organization, must be
 *  made publicly available under this license for as long as the redistribution is made available by
 *  the research entity.
 *
 *  4. Redistribution of this software, without modification, must refer to the software by the same
 *  designation. Redistribution of a modified version of this software (i) may not refer to the modified
 *  version by the same designation, or by any confusingly similar designation, and (ii) must refer to
 *  the underlying software originally provided by Alliance as "SolTrace". Except to comply with the
 *  foregoing, the term "SolTrace", or any confusingly similar designation may not be used to refer to
 *  any modified version of this software or any modified version of the underlying software originally
 *  provided by Alliance without the prior written consent of Alliance.
 *
 *  5. The name of the copyright holder, contributors, the United States Government, the United States
 *  Department of Energy, or any of their employees may not be used to endorse or promote products
 *  derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 *  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 *  FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER,
 *  CONTRIBUTORS, UNITED STATES GOVERNMENT OR UNITED STATES DEPARTMENT OF ENERGY, NOR ANY OF THEIR
 *  EMPLOYEES, BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 *  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 *  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************************************/

#include "trace.hpp"

#include <algorithm>
#include <cmath>
#include <ctime>
#include <string>
#include <vector>

#include "matvec.hpp"
#include "native_runner_types.hpp"
#include "treemesh.h"

// #define   Order 3
// #define   NumIterations 20
// #define   Epsilon 0.000001

#define sign(x) (x >= 0)

void FindElementHit(
	// stage info
	const int i,
	const TStage *Stage,
	const bool PT_override,
	const bool AsPowerTower,
	// element info
	const int nintelements,
	const vector<void *> &sunint_elements,
	const vector<void *> &reflint_elements,
	// ray info
	const int RayNumber,
	const bool in_multi_hit_loop,
	double (&PosRayStage)[3],
	double (&CosRayStage)[3],
	// outputs
	double (&LastPosRaySurfElement)[3],
	double (&LastCosRaySurfElement)[3],
	double (&LastDFXYZ)[3],
	uint_fast64_t &LastElementNumber,
	uint_fast64_t &LastRayNumber,
	double (&LastPosRaySurfStage)[3],
	double (&LastCosRaySurfStage)[3],
	int &ErrorFlag,
	int &LastHitBackSide,
	bool &StageHit)
{
	// Initialize Variables
	double LastPathLength = 1e99;
	int HitBackSide = 0;
	int InterceptFlag = 0;
	double DFXYZ[3] = {0.0, 0.0, 0.0};
	double PosRayElement[3] = {0.0, 0.0, 0.0};
	double CosRayElement[3] = {0.0, 0.0, 0.0};
	double PosRaySurfStage[3] = {0.0, 0.0, 0.0};
	double CosRaySurfStage[3] = {0.0, 0.0, 0.0};
	double PosRaySurfElement[3] = {0.0, 0.0, 0.0};
	double CosRaySurfElement[3] = {0.0, 0.0, 0.0};
	StageHit = false;

	for (uint_fast64_t j = 0; j < nintelements; j++)
	{
		TElement *Element; // = Stage->ElementList[j];
		if (i == 0 && !PT_override)
		{
			if (in_multi_hit_loop)
			{
				if (AsPowerTower)
					Element = (TElement *)reflint_elements.at(j);
				else
					Element = Stage->ElementList[j].get();
			}
			else
				Element = (TElement *)sunint_elements.at(j);
		}
		else
			Element = Stage->ElementList[j].get();

		// if (!Element->Enabled)
		// 	continue;

		//  {Transform ray to element[j] coord system of Stage[i]}
		TransformToLocal(PosRayStage, CosRayStage,
						 Element->Origin, Element->RRefToLoc,
						 PosRayElement, CosRayElement);

		ErrorFlag = 0;
		HitBackSide = 0;
		InterceptFlag = 0;
		double PathLength = 0;

		// increment position by tiny amount to get off the element
		// if tracing to the same element
		PosRayElement[0] = PosRayElement[0] + 1.0e-5 * CosRayElement[0];
		PosRayElement[1] = PosRayElement[1] + 1.0e-5 * CosRayElement[1];
		PosRayElement[2] = PosRayElement[2] + 1.0e-5 * CosRayElement[2];

		// {Determine if ray intersects element[j]; if so, Find intersection
		// point with surface of element[j] }
		DetermineElementIntersectionNew(Element,
										PosRayElement,
										CosRayElement,
										PosRaySurfElement,
										CosRaySurfElement,
										DFXYZ,
										&PathLength,
										&ErrorFlag,
										&InterceptFlag,
										&HitBackSide);

		if (InterceptFlag)
		{
			// {If hit multiple elements, this loop determines which one hit
			// first. Also makes sure that correct part of closed surface is
			// hit. Also, handles wavy, but close to flat zernikes and
			// polynomials correctly.}
			if (PathLength < LastPathLength)
			{
				if (PosRaySurfElement[2] <= Element->ZAperture ||
					Element->SurfaceIndex == 'm' ||
					Element->SurfaceIndex == 'M' ||
					Element->SurfaceIndex == 'r' ||
					Element->SurfaceIndex == 'R')
				{
					StageHit = true;
					LastPathLength = PathLength;
					CopyVec3(LastPosRaySurfElement, PosRaySurfElement);
					CopyVec3(LastCosRaySurfElement, CosRaySurfElement);
					CopyVec3(LastDFXYZ, DFXYZ);
					LastElementNumber = ((i == 0 && !PT_override) ? Element->element_number : j + 1); // mjw change from j index to element id
					LastRayNumber = RayNumber;
					TransformToReference(PosRaySurfElement, CosRaySurfElement,
										 Element->Origin, Element->RLocToRef,
										 PosRaySurfStage, CosRaySurfStage);

					CopyVec3(LastPosRaySurfStage, PosRaySurfStage);
					CopyVec3(LastCosRaySurfStage, CosRaySurfStage);
					LastHitBackSide = HitBackSide;
				}
			}
		}
	}
}

void ProcessInteraction(
	// system info
	TSystem *System,
	MTRand &myrng,
	const bool IncludeSunShape,
	TOpticalProperties *optics,
	const bool IncludeErrors,
	// stage info
	const int i,
	const TStage *Stage,
	const int k,
	// ray info
	const uint_fast64_t MultipleHitCount,
	double (&LastDFXYZ)[3],
	// Outputs
	double (&LastCosRaySurfElement)[3],
	int &ErrorFlag,
	double (&CosRayOutElement)[3],
	double (&LastPosRaySurfElement)[3],
	double (&PosRayOutElement)[3],
	int &myrng_counter)
{
	// Initialize
	double CosIn[3] = {0.0, 0.0, 0.0};
	double CosOut[3] = {0.0, 0.0, 0.0};

	if (!Stage->Virtual)
	{
		// change to account for first hit only in primary stage 8-11-31
		if (IncludeSunShape && i == 0 && MultipleHitCount == 1)
		{
			// Apply sunshape to UNPERTURBED ray at intersection point
			// only apply sunshape error once for primary stage
			CopyVec3(CosIn, LastCosRaySurfElement);
			// sun shape
			Errors(myrng, CosIn, 1, &System->Sun,
				   Stage->ElementList[k].get(), optics, CosOut, LastDFXYZ);
			CopyVec3(LastCosRaySurfElement, CosOut);
		}

		//{Determine interaction at surface and direction of perturbed ray}
		ErrorFlag = 0;

		// {Apply surface normal errors to surface normal before interaction
		// ray at intersection point - Wendelin 11-23-09}
		if (IncludeErrors)
		{
			CopyVec3(CosIn, CosRayOutElement);
			// surface normal errors
			SurfaceNormalErrors(myrng, LastDFXYZ, optics, CosOut);
			myrng_counter++;
			CopyVec3(LastDFXYZ, CosOut);
		}

		Interaction(myrng, LastPosRaySurfElement, LastCosRaySurfElement,
					LastDFXYZ, Stage->ElementList[k]->InteractionType,
					optics, 630.0, PosRayOutElement, CosRayOutElement,
					&ErrorFlag);
		myrng_counter++;

		// {Apply specularity optical error to PERTURBED (i.e. after
		// interaction) ray at intersection point}
		if (IncludeErrors)
		{
			if (optics->DistributionType == 'F' ||
				optics->DistributionType == 'f')
			{
				// Apply diffuse errors relative to surface normal
				CopyVec3(CosIn, LastDFXYZ);
			}
			else
			{
				// Apply all other errors relative to the specularly-reflected
				// direction
				CopyVec3(CosIn, CosRayOutElement);
			}
			// optical errors
			Errors(myrng, CosIn, 2, &System->Sun,
				   Stage->ElementList[k].get(), optics, CosOut, LastDFXYZ);
			myrng_counter++;
			CopyVec3(CosRayOutElement, CosOut);
		}
	}
}

// PT Optimization Methods and Structs

struct eprojdat
{
	TElement *el_addr;
	double d_proj;
	double az;
	double zen;

	eprojdat() {};
	eprojdat(TElement *e, double d, double a, double z)
	{
		el_addr = e;
		d_proj = d;
		az = a;
		zen = z;
	};
};

// Comparison function for sorting vector of eprojdat
static bool eprojdat_compare_refactored(const eprojdat &A, const eprojdat &B)
{
	return A.d_proj > B.d_proj;
};

void SetupPTOptimizations(
	// system info
	TSystem *System, const bool AsPowerTower,

	// outputs
	st_hash_tree &sun_hash, st_hash_tree &rec_hash, double (&reccm_helio)[3])
{
	// Calculate the center of mass of the receiver stage (StageList[1]) in
	// heliostat stage coordinates.
	double reccm[] = {0., 0., 0.};
	int nelrec = 0;
	if (AsPowerTower)
	{
		for (uint_fast64_t j = 0; j < System->StageList[1]->ElementList.size(); j++)
		{
			TElement *el = System->StageList[1]->ElementList.at(j).get();

			// if (!el->Enabled)
			// 	continue;

			nelrec++;

			for (int jj = 0; jj < 3; jj++)
				reccm[jj] += el->Origin[jj];
		}
		for (int jj = 0; jj < 3; jj++)
			reccm[jj] /= (double)nelrec; // average

		// Transform to reference
		double dum1[] = {0., 0., 1.};
		double dum2[3];
		double reccm_global[3];
		TransformToReference(reccm, dum1, System->StageList[1]->Origin,
							 System->StageList[1]->RLocToRef, reccm_global, dum2);

		// Transform to local (heliostat). reccm_helio is the x,y,z position of the receiver centroid in heliostat stage coordinates.
		TransformToLocal(reccm_global, dum1, System->StageList[0]->Origin,
						 System->StageList[0]->RRefToLoc, reccm_helio, dum2);
	}
	// Create an array that stores the element address and the projected size in polar coordinates
	vector<eprojdat> el_proj_dat;
	el_proj_dat.reserve(System->StageList[0]->ElementList.size());

	// calculate the smallest zone size. This should be on the order of the largest element in the stage.
	// load stage 0 elements into the mesh
	double d_elm_max = -9.e9;

	for (uint_fast64_t i = 0; i < System->StageList[0]->ElementList.size(); i++)
	{
		TElement *el = System->StageList[0]->ElementList.at(i).get();

		el->element_number = i + 1; // use index for element number

		double d_elm;

		switch (el->ShapeIndex)
		{
		// circular aperture
		case 'c':
		case 'C':
			// hexagonal aperture
		case 'h':
		case 'H':
			// triangular aperture
		case 't':
		case 'T':
			d_elm = el->ParameterA;
			break;
			// rectangular aperture
		case 'r':
		case 'R':
			d_elm = sqrt(el->ParameterA * el->ParameterA + el->ParameterB * el->ParameterB);
			break;
			// annular aperture
		case 'a':
		case 'A':
			d_elm = el->ParameterB;
			break;
		case 'l':
		case 'L':
			// off axis aperture section of line focus trough  or cylinder
			d_elm = sqrt(el->ParameterB * el->ParameterB * 4. + el->ParameterC * el->ParameterC);
			break;
			// Irregular triangle
		case 'i':
		case 'I':
			// irregular quadrilateral
		case 'q':
		case 'Q':
		{
			double xmax = fmax(el->ParameterA, fmax(el->ParameterC, el->ParameterE));
			double xmin = fmin(el->ParameterA, fmin(el->ParameterC, el->ParameterE));
			double ymax = fmax(el->ParameterB, fmax(el->ParameterD, el->ParameterF));
			double ymin = fmin(el->ParameterB, fmin(el->ParameterD, el->ParameterF));

			if (el->ShapeIndex == 'q' || el->ShapeIndex == 'Q')
			{
				xmax = fmax(xmax, el->ParameterG);
				xmin = fmin(xmin, el->ParameterG);
				ymax = fmax(ymax, el->ParameterH);
				ymin = fmin(ymin, el->ParameterH);
			}

			double dx = xmax - xmin;
			double dy = ymax - ymin;

			d_elm = sqrt(dx * dx + dy * dy);

			break;
		}
		default:
			break;
		}

		d_elm_max = fmax(d_elm_max, d_elm);

		if (AsPowerTower)
		{
			// Calculate the distance from the receiver to the element and the max projected size
			double dX[3];
			for (int jj = 0; jj < 3; jj++)
				dX[jj] = el->Origin[jj] - reccm_helio[jj]; // vector from receiver to heliostat (not unitized)
			double r_elm = 0.;
			for (int jj = 0; jj < 3; jj++)
				r_elm += dX[jj] * dX[jj];
			r_elm = sqrt(r_elm);			   // vector length
			double d_elm_proj = d_elm / r_elm; // Projected size of the element from the view of the receiver (radians)

			// calculate az,zen coordinate
			double az, zen;
			az = atan2(dX[0] / r_elm, dX[1] / r_elm); // Az coordinate of the heliostat from the receiver's perspective
			zen = asin(dX[2] / r_elm);				  // Zen coordinate """"

			el_proj_dat.push_back(eprojdat(el, d_elm_proj, az, zen));
		}
	}

	if (AsPowerTower)
	{
		// Sort the polar projections by size, largest to smallest
		std::sort(el_proj_dat.begin(), el_proj_dat.end(), eprojdat_compare_refactored);
	}

	// set up the layout data object that provides configuration details for
	// the hash tree
	KDLayoutData sun_ld;
	sun_ld.xlim[0] = System->Sun.MinXSun;
	sun_ld.xlim[1] = System->Sun.MaxXSun;
	sun_ld.ylim[0] = System->Sun.MinYSun;
	sun_ld.ylim[1] = System->Sun.MaxYSun;
	sun_ld.min_unit_dx = d_elm_max;
	sun_ld.min_unit_dy = d_elm_max;

	sun_hash.create_mesh(sun_ld);

	// load stage 0 elements into the mesh
	for (uint_fast64_t i = 0; i < System->StageList[0]->ElementList.size(); i++)
	{
		TElement *el = System->StageList[0]->ElementList.at(i).get();
		sun_hash.add_object((void *)el, el->PosSunCoords[0], el->PosSunCoords[1]);
	}

	// calculate and associate neighbors with each zone
	sun_hash.add_neighborhood_data();

	if (AsPowerTower)
	{
		// Set things up for the polar coordinate tree
		KDLayoutData rec_ld;
		rec_ld.xlim[0] = -M_PI;
		rec_ld.xlim[1] = M_PI;
		rec_ld.ylim[0] = -M_PI / 2.;
		rec_ld.ylim[1] = M_PI / 2.;
		// use smallest element to set the minimum size
		rec_ld.min_unit_dx = rec_ld.min_unit_dy = el_proj_dat.back().d_proj; // radians at equator

		rec_hash.create_mesh(rec_ld);

		// load stage 0 elements into the receiver mesh in the order of largest projection to smallest
		for (int i = 0; i < el_proj_dat.size(); i++)
		{
			eprojdat *D = &el_proj_dat.at(i);

			// Calculate the angular span of the element
			double angspan[2];
			double adjmult = 1.5;
			angspan[0] = D->d_proj / cos(fabs(D->zen)) * adjmult; // azimuthal span
			angspan[0] = fmin(angspan[0], 2. * M_PI);			  // limit to circumference
			angspan[1] = D->d_proj / M_PI * adjmult;			  // zenithal span
			rec_hash.add_object((void *)D->el_addr, D->az, D->zen, angspan);
		}

		// associate neighbors with each zone
		rec_hash.add_neighborhood_data();
	}
}

uint_fast64_t GetPTElements(
	// system info
	const bool AsPowerTower,

	// Stage info
	const TStage *Stage, const int i,

	// Ray info
	const bool in_multi_hit_loop, const double (&PosRayStage)[3],
	const double (&reccm_helio)[3], st_hash_tree &rec_hash,

	const vector<void *> &sunint_elements,

	// Outputs
	vector<void *> &reflint_elements,
	bool &has_elements)
{
	uint_fast64_t nintelements = 0;

	if (i == 0)
	{
		if (in_multi_hit_loop)
		{
			if (AsPowerTower)
			{
				//>=Second time through - checking for first stage multiple element interactions

				// get ray position in receiver polar coordinates
				double raypvec[3];
				for (int jj = 0; jj < 3; jj++)
					raypvec[jj] = PosRayStage[jj] - reccm_helio[jj];
				double raypvecmag = sqrt(raypvec[0] * raypvec[0] + raypvec[1] * raypvec[1] + raypvec[2] * raypvec[2]);
				double raypol[2];
				raypol[0] = atan2(raypvec[0], raypvec[1]);
				raypol[1] = asin(raypvec[2] / raypvecmag);
				// get elements in the vicinity of the ray's polar coordinates
				reflint_elements.clear();
				rec_hash.get_all_data_at_loc(reflint_elements, raypol[0], raypol[1]);
				nintelements = reflint_elements.size();
				has_elements = nintelements > 0;
			}
			else
			{
				nintelements = Stage->ElementList.size();
			}
		}
		else
		{
			// First time through - checking for sun ray intersections
			if (has_elements)
				nintelements = sunint_elements.size();
			else
				nintelements = 0;
		}
	}
	else
		nintelements = Stage->ElementList.size();

	return nintelements;
}

// Trace method
bool trace_native(
	TSystem *System,
	unsigned int seed,
	uint_fast64_t NumberOfRays,
	uint_fast64_t MaxNumberOfRays,
	bool IncludeSunShape,
	bool IncludeErrors,
	bool AsPowerTower)
{
	// Determine if PT optimizations should be applied
	bool PT_override = false;
	if (System->StageList.size() > 0 &&
		(System->StageList[0]->ElementList.size() < 10 || System->StageList.size() == 1))
	{
		PT_override = true;
	}

	// Initialize variables
	MTRand myrng(seed);
	int myrng_counter = 0;

	// Initialize Internal State Variables
	uint_fast64_t RayNumber = 1; // Ray Number of current ray
	bool PreviousStageHasRays = false;
	uint_fast64_t LastRayNumberInPreviousStage = NumberOfRays;

	// // Check Inputs
	// if (NumberOfRays < 1)
	// {
	// 	System->errlog("invalid number of rays: %d", NumberOfRays);
	// 	return false;
	// }
	// if (System->StageList.size() < 1)
	// {
	// 	System->errlog("no stages defined.");
	// 	return false;
	// }

	// Define IncomingRays
	std::vector<GlobalRay_refactored> IncomingRays; // Vector of rays from previous stage, going into next stage
	IncomingRays.resize(NumberOfRays);
	// try
	// {
	// 	IncomingRays.resize(NumberOfRays);
	// }
	// catch (std::exception& e) {
	// 	System->errlog("Incoming rays resize exception: %d, '%s'", NumberOfRays, e.what());
	// 	return false;
	// }

	// Initialize Sun
	double PosSunStage[3] = {0.0, 0.0, 0.0};
	if (!SunToPrimaryStage(System,
						   System->StageList[0].get(),
						   &System->Sun,
						   PosSunStage))
		return false;

	// Calculate hash tree for reflection to receiver plane(polar coordinates).
	st_hash_tree sun_hash;
	st_hash_tree rec_hash;
	double reccm_helio[3]; // receiver centroid in heliostat field coordinates
	if (!PT_override)
	{
		SetupPTOptimizations(System, AsPowerTower, sun_hash,
							 rec_hash, reccm_helio);
	}

	// Start the clock
	clock_t startTime = clock();
	int rays_per_callback_estimate = 50;
	uint_fast64_t RaysTracedTotal = 0;

	// Loop through stages
	for (uint_fast64_t i = 0; i < System->StageList.size(); i++)
	{
		// Check if previous stage has rays
		bool StageHasRays = true;
		if (i > 0 && PreviousStageHasRays == false)
		{
			StageHasRays = false;
		}

		// Get Current Stage
		TStage *Stage = System->StageList[i].get();

		// Initialize stage variables
		uint_fast64_t StageDataArrayIndex = 0;
		uint_fast64_t PreviousStageDataArrayIndex = 0;

		// Loop through rays
		while (StageHasRays)
		{
			// Initialize Global Coordinates
			double PosRayGlob[3] = {0.0, 0.0, 0.0};
			double CosRayGlob[3] = {0.0, 0.0, 0.0};

			// Initialize Stage Coordinates
			double PosRayStage[3] = {0.0, 0.0, 0.0};
			double CosRayStage[3] = {0.0, 0.0, 0.0};

			// Initialize PT Optimization variables
			bool has_elements = true;
			vector<void *> sunint_elements;

			// Get Ray
			if (i == 0)
			{
				// Make ray (if first stage)
				double PosRaySun[3];
				GenerateRay(myrng, PosSunStage, Stage->Origin,
							Stage->RLocToRef, &System->Sun,
							PosRayGlob, CosRayGlob, PosRaySun);
				myrng_counter++;
				System->SunRayCount++;

				// If using PT optimizations, check if stage has elements
				// that could interact with ray
				if (!PT_override)
				{
					has_elements =
						sun_hash.get_all_data_at_loc(sunint_elements,
													 PosRaySun[0],
													 PosRaySun[1]);
				}
			}
			else
			{
				// Get ray from previous stage
				RayNumber = IncomingRays[StageDataArrayIndex].Num;
				CopyVec3(PosRayGlob, IncomingRays[StageDataArrayIndex].Pos);
				CopyVec3(CosRayGlob, IncomingRays[StageDataArrayIndex].Cos);
				StageDataArrayIndex++;
			}

			// transform the global incoming ray to local stage coordinates
			TransformToLocal(PosRayGlob, CosRayGlob,
							 Stage->Origin, Stage->RRefToLoc,
							 PosRayStage, CosRayStage);

			// // Update callback
			// if (callback != 0
			// 	&& RaysTracedTotal++ % rays_per_callback_estimate == 0)
			// {
			// 	if (RaysTracedTotal > 1)
			// 	{
			// 		//update how often to call this
			// 		double msec_per_ray = 1000. * (clock() - startTime) / CLOCKS_PER_SEC / (double)(RaysTracedTotal > 0 ? RaysTracedTotal : 1);
			// 		//set the new callback estimate to be about 50 ms
			// 		rays_per_callback_estimate = (int)(200. / msec_per_ray);
			// 		//limit to something reasonable
			// 		rays_per_callback_estimate = rays_per_callback_estimate < 5 ? 5 : rays_per_callback_estimate;
			// 	}

			// 	//do the callback
			// 	if (!(*callback)(RaysTracedTotal, RayNumber,
			// 		LastRayNumberInPreviousStage, i + 1,
			// 		System->StageList.size(), cbdata))
			// 		return true;
			// }

			// Initialize internal variables for ray intersection tracing
			bool RayInStage = true;
			bool in_multi_hit_loop = false;
			double LastPosRaySurfElement[3] = {0.0, 0.0, 0.0};
			double LastCosRaySurfElement[3] = {0.0, 0.0, 0.0};
			double LastPosRaySurfStage[3] = {0.0, 0.0, 0.0};
			double LastCosRaySurfStage[3] = {0.0, 0.0, 0.0};
			double LastDFXYZ[3] = {0.0, 0.0, 0.0};
			uint_fast64_t LastElementNumber = 0;
			uint_fast64_t LastRayNumber = 0;
			int ErrorFlag;
			int LastHitBackSide;
			bool StageHit;
			int MultipleHitCount = 0;
			double PosRayOutElement[3] = {0.0, 0.0, 0.0};
			double CosRayOutElement[3] = {0.0, 0.0, 0.0};

			// Start Loop to trace ray until it leaves stage
			bool RayIsAbsorbed = false;
			while (RayInStage)
			{
				// Set number of elements to search through
				uint_fast64_t nintelements = 0;
				vector<void *> reflint_elements;
				if (!PT_override) // if using opt AND first stage
				{
					nintelements = GetPTElements(AsPowerTower, Stage, i,
												 in_multi_hit_loop, PosRayStage,
												 reccm_helio, rec_hash,
												 sunint_elements,
												 reflint_elements, has_elements);
				}
				else
				{
					nintelements = Stage->ElementList.size();
				}

				// Find the element the ray hits
				FindElementHit(i, Stage, PT_override, AsPowerTower,
							   nintelements, sunint_elements, reflint_elements,
							   RayNumber, in_multi_hit_loop,
							   PosRayStage, CosRayStage,

							   LastPosRaySurfElement, LastCosRaySurfElement, LastDFXYZ,
							   LastElementNumber, LastRayNumber,
							   LastPosRaySurfStage, LastCosRaySurfStage,
							   ErrorFlag, LastHitBackSide, StageHit);

				// Breakout if ray left stage
				if (!StageHit)
				{
					RayInStage = false;
					break;
				}

				// Add ray to Stage RayData
				TRayData::ray_t *p_ray = Stage->RayData.Append(LastPosRaySurfStage,
															   LastCosRaySurfStage,
															   LastElementNumber,
															   i + 1,
															   LastRayNumber);

				// Check p_ray saved correctly
				if (!p_ray)
				{
					System->errlog("Failed to save ray data at index %d", Stage->RayData.Count() - 1);
					return false;
				}

				// Skipping LastElementNumber == 0 check

				// Increment MultipleHitCount
				MultipleHitCount++;

				// Get optics and check for absorption
				TOpticalProperties *optics = 0;
				if (Stage->Virtual)
				{
					// If stage is virtual, there is no interaction
					CopyVec3(PosRayOutElement, LastPosRaySurfElement);
					CopyVec3(CosRayOutElement, LastCosRaySurfElement);
				}
				else
				{
					// trace through the interaction
					TElement *optelm = Stage->ElementList[p_ray->element - 1].get();

					if (LastHitBackSide)
						optics = &optelm->Optics->Back;
					else
						optics = &optelm->Optics->Front;

					double TestValue;
					double UnitLastDFXYZ[3] = {0.0, 0.0, 0.0};
					double IncidentAngle = 0;
					switch (optelm->InteractionType)
					{
					case 1: // refraction
						if (optics->UseTransmissivityTable)
						{
							int npoints = optics->TransmissivityTable.size();
							int m = 0;

							UnitLastDFXYZ[0] = -LastDFXYZ[0] / sqrt(DOT(LastDFXYZ, LastDFXYZ));
							UnitLastDFXYZ[1] = -LastDFXYZ[1] / sqrt(DOT(LastDFXYZ, LastDFXYZ));
							UnitLastDFXYZ[2] = -LastDFXYZ[2] / sqrt(DOT(LastDFXYZ, LastDFXYZ));
							IncidentAngle = acos(DOT(LastCosRaySurfElement, UnitLastDFXYZ)) * 1000.; //[mrad]
							if (IncidentAngle >= optics->TransmissivityTable[npoints - 1].angle)
							{
								TestValue = optics->TransmissivityTable[npoints - 1].trans;
							}
							else
							{
								while (optics->TransmissivityTable[m].angle < IncidentAngle)
									m++;

								if (m == 0)
									TestValue = optics->TransmissivityTable[m].trans;
								else
									TestValue = (optics->TransmissivityTable[m].trans + optics->TransmissivityTable[m - 1].trans) / 2.0;
							}
						}
						else
							TestValue = optics->Transmissivity;
						break;
					case 2: // reflection

						if (optics->UseReflectivityTable)
						{
							int npoints = optics->ReflectivityTable.size();
							int m = 0;
							UnitLastDFXYZ[0] = -LastDFXYZ[0] / sqrt(DOT(LastDFXYZ, LastDFXYZ));
							UnitLastDFXYZ[1] = -LastDFXYZ[1] / sqrt(DOT(LastDFXYZ, LastDFXYZ));
							UnitLastDFXYZ[2] = -LastDFXYZ[2] / sqrt(DOT(LastDFXYZ, LastDFXYZ));
							IncidentAngle = acos(DOT(LastCosRaySurfElement, UnitLastDFXYZ)) * 1000.; //[mrad]
							if (IncidentAngle >= optics->ReflectivityTable[npoints - 1].angle)
							{
								TestValue = optics->ReflectivityTable[npoints - 1].refl;
							}
							else
							{
								while (optics->ReflectivityTable[m].angle < IncidentAngle)
									m++;

								if (m == 0)
									TestValue = optics->ReflectivityTable[m].refl;
								else
									TestValue = (optics->ReflectivityTable[m].refl + optics->ReflectivityTable[m - 1].refl) / 2.0;
							}
						}
						else
							TestValue = optics->Reflectivity;
						break;
					default:
						System->errlog("Bad optical interaction type = %d (stage %d)", i, optelm->InteractionType);
						return false;
					}

					//  {Apply MonteCarlo probability of absorption. Limited for now, but can make more complex later on if desired}
					if (TestValue <= myrng())
					{
						myrng_counter++;
						// ray was fully absorbed, so indicate by negating the element number
						p_ray->element = 0 - p_ray->element;
						RayIsAbsorbed = true;
						break;
					}
				}

				// Process Interaction
				int k = abs(p_ray->element) - 1;
				ProcessInteraction(System, myrng, IncludeSunShape, optics, IncludeErrors,
								   i, Stage, k,
								   MultipleHitCount, LastDFXYZ,
								   LastCosRaySurfElement, ErrorFlag,
								   CosRayOutElement, LastPosRaySurfElement,
								   PosRayOutElement, myrng_counter);

				// Transform ray back to stage coordinate system
				TransformToReference(PosRayOutElement, CosRayOutElement,
									 Stage->ElementList[k]->Origin, Stage->ElementList[k]->RLocToRef,
									 PosRayStage, CosRayStage);
				TransformToReference(PosRayStage, CosRayStage,
									 Stage->Origin, Stage->RLocToRef,
									 PosRayGlob, CosRayGlob);

				// Break out if multiple hits are not allowed
				if (!Stage->MultiHitsPerRay)
				{
					StageHit = false;
					break;
				}
				else
				{
					in_multi_hit_loop = true;
				}
			}

			// Handle if Ray was absorbed
			if (RayIsAbsorbed)
			{
				// ray was fully absorbed
				if (RayNumber == LastRayNumberInPreviousStage)
				{
					PreviousStageHasRays = false;
					if (PreviousStageDataArrayIndex > 0)
					{
						PreviousStageDataArrayIndex--;
						PreviousStageHasRays = true;
					}
					break;
				}
				else
				{
					if (i == 0)
					{
						if (RayNumber == NumberOfRays)
							break;
						else
							RayNumber++;
					}

					// Next ray in loop
					continue;
				}
			}

			// !StageHit logic goes here
			if (StageHit == true)
			{
				// This shouldn't happen...
			}

			// Ray has left the stage
			bool FlagMiss = false;
			if (i == 0)
			{
				if (MultipleHitCount == 0)
				{
					// Ray in first stage missed stage entirely
					// Generate new ray
					continue;
				}
				else
				{
					// Ray hit an element, so save it for next stage
					CopyVec3(IncomingRays[PreviousStageDataArrayIndex].Pos, PosRayGlob);
					CopyVec3(IncomingRays[PreviousStageDataArrayIndex].Cos, CosRayGlob);
					IncomingRays[PreviousStageDataArrayIndex].Num = RayNumber;

					// Is Ray the last in the stage?
					if (RayNumber == NumberOfRays)
					{
						StageHasRays = false;
						break;
					}

					PreviousStageDataArrayIndex++;
					PreviousStageHasRays = true;

					// Move on to next ray
					RayNumber++;
					continue;
				}
			}
			else
			{
				// After the first stage
				// Ray hit element OR is traced through stage
				if (Stage->TraceThrough || MultipleHitCount > 0)
				{
					// Ray is saved for the next stage
					CopyVec3(IncomingRays[PreviousStageDataArrayIndex].Pos, PosRayGlob);
					CopyVec3(IncomingRays[PreviousStageDataArrayIndex].Cos, CosRayGlob);
					IncomingRays[PreviousStageDataArrayIndex].Num = RayNumber;

					// Check if ray is last in stage
					if (RayNumber == LastRayNumberInPreviousStage)
					{
						StageHasRays = false;
						break;
					}

					PreviousStageDataArrayIndex++;
					PreviousStageHasRays = true;

					if (MultipleHitCount == 0)
					{
						FlagMiss = true;
					}

					// Go to next ray
					continue;
				}
				// Ray missed stage entirely and is not traced
				else
				{
					FlagMiss = true;
				}

				// Handle FlagMiss condition (
				if (FlagMiss == true)
				{
					LastElementNumber = 0;
					LastRayNumber = RayNumber;
					CopyVec3(LastPosRaySurfStage, PosRayStage);
					CopyVec3(LastCosRaySurfStage, CosRayStage);

					// Copying this here to handle FlagMiss condition
					TRayData::ray_t *p_ray = Stage->RayData.Append(LastPosRaySurfStage,
																   LastCosRaySurfStage,
																   LastElementNumber,
																   i + 1,
																   LastRayNumber);

					if (RayNumber == LastRayNumberInPreviousStage)
					{
						if (!Stage->TraceThrough)
						{
							PreviousStageHasRays = false;
							if (PreviousStageDataArrayIndex > 0)
							{
								PreviousStageHasRays = true;
								PreviousStageDataArrayIndex--; // last ray was previous one
							}
						}

						// Exit stage
						StageHasRays = false;
						break;
					}
					else
					{
						if (i == 0)
							RayNumber++; // generate new sun ray

						// Start new ray
						continue;
					}
				}
			}
		}

		// EndStage section...

		// skipping save_st_data logic

		if (!PreviousStageHasRays)
		{
			LastRayNumberInPreviousStage = 0;
			continue; // No rays to carry forward
		}

		if (PreviousStageDataArrayIndex < IncomingRays.size())
		{
			LastRayNumberInPreviousStage = IncomingRays[PreviousStageDataArrayIndex].Num;
			if (LastRayNumberInPreviousStage == 0)
			{
				size_t pp = IncomingRays[PreviousStageDataArrayIndex - 1].Num;
				// System->errlog("LastRayNumberInPreviousStage=0, stage %d, PrevIdx=%d, CurIdx=%d, pp=%d", i + 1,
				// 	PreviousStageDataArrayIndex, StageDataArrayIndex, pp);
				return false;
			}
		}
		else
		{
			// System->errlog("Invalid PreviousStageDataArrayIndex: %u, @ stage %d",
			// 	PreviousStageDataArrayIndex, i + 1);
			return false;
		}
	}

	return true;
}

// #define RANGEN myrng

void GenerateRay(
	MTRand &myrng,
	double PosSunStage[3],
	double Origin[3],
	double RLocToRef[3][3],
	TSun *Sun,
	double PosRayGlobal[3],
	double CosRayGlobal[3],
	double PosRaySun[3])
{
	/*{This procedure generates a randomly located ray in the x-y plane of the sun coordinate system in
	 the z direction of the sun coord. system, checks to see that the ray is within the region of interest
	 defined by the spatial extent of the elements of Stage as seen from the sun
	 and ultimately transforms that ray to the global coord. system.   The z-axis of the sun coord. system points
	 towards the Stage coord. system origin.

	 Input
		   - Seed = Seed for random number generator
		   - Sun = Sun data record of type TSun
		   - Origin = Primary Stage origin
		   - RLocToRef = transformation matrix from local to reference frame
	 Output
		   - PosRayGlobal = Position of ray in Global coordinate system
		   - CosRayGlobal = Direction cosines of ray in Global coordinate system} */

	double XRaySun = 0.0, YRaySun = 0.0, ZRaySun = 0.0;
	double CosRaySun[3] = {0.0, 0.0, 0.0};
	double PosRayStage[3] = {0.0, 0.0, 0.0};
	double CosRayStage[3] = {0.0, 0.0, 0.0};
	int NegPosSign = 0;
	PosRaySun[0] = 0.;
	PosRaySun[1] = 0.;
	PosRaySun[2] = 0.;

	// ZRaySun := 0.0;  //Origin of rays in xy plane of sun coord system.
	ZRaySun = -10000.0; // changed 5/1/00.  rays originate from well bebind the sun coordinate system xy
						//  plane which has been translated to primary stage origin.         This value has been reduced signficantly because of numerical issues in tracing rays from sun
						//  to the closer form solution for a cylinder.  It used to 1e6 and has been reduced to 1e4, which should still be sufficient.   10-26-09 Wendelin

	//{Generate random rays inside of region of interest or from point source}

	if (Sun->PointSource) // fixed this on 3-18-13
	{
		PosRayGlobal[0] = Sun->Origin[0];
		PosRayGlobal[1] = Sun->Origin[1];
		PosRayGlobal[2] = Sun->Origin[2];

		if (myrng() <= 0.5)
			NegPosSign = -1;
		else
			NegPosSign = 1;

		CosRayGlobal[0] = NegPosSign * myrng(); // random direction for x part of ray vector

		if (myrng() <= 0.5)
			NegPosSign = -1;
		else
			NegPosSign = 1;

		CosRayGlobal[1] = NegPosSign * myrng(); // random direction for y part of ray vector

		if (myrng() <= 0.5)
			NegPosSign = -1;
		else
			NegPosSign = 1;

		CosRayGlobal[2] = NegPosSign * myrng(); // random direction for z part of ray vector

		double CosRayGMag = sqrt(CosRayGlobal[0] * CosRayGlobal[0] +
								 CosRayGlobal[1] * CosRayGlobal[1] +
								 CosRayGlobal[2] * CosRayGlobal[2]);

		CosRayGlobal[0] = CosRayGlobal[0] / CosRayGMag; // obtain unit vector by dividing by magnitude
		CosRayGlobal[1] = CosRayGlobal[1] / CosRayGMag;
		CosRayGlobal[2] = CosRayGlobal[2] / CosRayGMag;
	}
	else
	{
		// following changed on 09/26/05 to more efficiently generate rays relative to element center of mass in primary stage
		/*{XRaySun := 2.0*MaxRad*ran3(Seed) - MaxRad;  //ran3 produces results independent of platform.
		YRaySun := 2.0*MaxRad*ran3(Seed) - MaxRad;
		if (XRaySun*XRaySun + YRaySun*YRaySun) > MaxRad*MaxRad then goto GENRAY;
		XRaySun := Xcm + XRaySun;  //adjust location of generated rays about element center of mass
		YRaySun := Ycm + YRaySun;}*/

		XRaySun = Sun->MinXSun + (Sun->MaxXSun - Sun->MinXSun) * myrng(); // uses a rectangular region of interest about the primary
		YRaySun = Sun->MinYSun + (Sun->MaxYSun - Sun->MinYSun) * myrng(); // stage. Added 09/26/05

		//{Offload ray location and direction cosines into sun array}
		PosRaySun[0] = XRaySun;
		PosRaySun[1] = YRaySun;
		PosRaySun[2] = ZRaySun;
		CosRaySun[0] = 0.0;
		CosRaySun[1] = 0.0;
		CosRaySun[2] = 1.0;

		//{Transform ray locations and dir cosines into Stage system}
		TransformToReference(PosRaySun, CosRaySun, PosSunStage, Sun->RLocToRef, PosRayStage, CosRayStage);

		//{Transform ray locations and dir cosines into global system}
		TransformToReference(PosRayStage, CosRayStage, Origin, RLocToRef, PosRayGlobal, CosRayGlobal);
	}

	return;
}

inline double sqr(double x) { return (x) * (x); }

void Interaction(
	MTRand &myrng,
	double PosXYZ[3],
	double CosKLM[3],
	double DFXYZ[3],
	int InteractionType,
	TOpticalProperties *Opticl,
	double Wavelength,
	double PosOut[3],
	double CosOut[3],
	int *ErrorFlag)
{
	/* {Purpose: To compute the direction cosines of the ray due to optical interaction
			   at the intersection point of the ray with the surface
		 Input - PosXYZ[2] = X,Y,Z coordinates of intersection point.
				 DFXYZ     = direction numbers for the surface normal at the
							 intersection point (partial derivatives with respect
							 to X,Y,Z of surface equation)
				 InteractionType = Optical interaction type indicator
						   = 1, refraction
						   = 2, reflection
						   = 3, aperture stop
						   = 4, diffraction, transmission grating
						   = 5, diffraction, reflection grating
				 CosKLM[2] = direction cosines of incident ray
				 Opticl    = record of optical properties
					   .RefractiveIndex[4] = Refractive index of incident and outgoing medium
									   [0] = real part of incident medium refractive index
									   [1] = imaginary part of ""
									   [2] = real part of outgoing medium refractive index
									   [4] = imaginary part of ""
					   .ApertureStopOrGratingType
										   for InteractionType = 3, aperture stop
											   = 1, slit
											   = 2, elliptical
										   for InteractionType = 4,5 grating
											   = 1, planes parallel to Y-Z plane
											   = 2, concentric cylinders centered about Z-axis
					   .DiffractionOrder = integral order of diffraction for InteractionTypes=4,5, grating
					   .AB12[4] = coefficients of polynomial specifying grating spacing for InteractionTypes=4,5
							[0] = lower X limit, ApertureStopOrGratingType = 1
								  semi-X axis, ApertureStopOrGratingType = 2
							[1] = lower Y limit, ApertureStopOrGratingType = 1
								  semi-Y axis, ApertureStopOrGratingType = 2
							[2] = upper X limit, ApertureStopOrGratingType = 1
								  unused, ApertureStopOrGratingType = 2
							[4] = upper Y limit, ApertureStopOrGratingType = 1
								  unused, ApertureStopOrGratingType = 2
				 Wavelength = wavelength of ray

		 Output - PosOut[2] = position of ray after optical interaction
				  CosOut[2] = direction cosines of ray after optical interaction
				  ErrorFlag = Error flag indicating successful interaction
	}*/

	int i = 0;
	double CosUVW[3] = {0.0, 0.0, 0.0};
	int NIter = 0, IType = 0, NMord = 0;
	double Epsilon = 0.0, Refr1 = 0.0, Refr2 = 0.0, RMU = 0.0, RM2 = 0.0;
	double D2 = 0.0, B = 0.0, A = 0.0, A2 = 0.0;
	double Gamn = 0.0, Gamn1 = 0.0;
	double X = 0.0, Y = 0.0, A1 = 0.0, B1 = 0.0, Ellips = 0.0, B2 = 0.0;
	double RK = 0.0, RL = 0.0, RM = 0.0, Denom, U = 0, V = 0, W = 0;
	double Varr = 0, GFactr = 0, Rho2 = 0.0, Rho = 0.0, Term = 0.0, G = 0.0, D = 0.0, XX = 0.0, Ordiff = 0.0, RLamda = 0.0;
	double Rave = 0.0, Rs = 0.0, Rp = 0.0;
	double UnitDFXYZ[3] = {0.0, 0.0, 0.0}, IncidentAngle = 0.0;

	NIter = 10;
	Epsilon = 0.000005;

	*ErrorFlag = 0;
	for (i = 0; i < 3; i++)
		PosOut[i] = PosXYZ[i];

	switch (InteractionType)
	{

		/*{  InteractionType = 1, Refraction
		===============================================================================}*/
	case 1:
	{
		Refr1 = Opticl->RefractiveIndex[0];
		Refr2 = Opticl->RefractiveIndex[2];
		RMU = Refr1 / Refr2;
		D2 = DOT(DFXYZ, DFXYZ);
		B = (RMU * RMU - 1.0) / D2;
		A = RMU * DOT(CosKLM, DFXYZ) / D2;
		A2 = A * A;
		if (B > A2) // Total internal reflection
		{
			A = DOT(CosKLM, DFXYZ) / DOT(DFXYZ, DFXYZ);
			for (i = 0; i < 3; i++)
				CosOut[i] = CosKLM[i] - 2.0 * A * DFXYZ[i];
			return;
		}

		// fresnel equations
		UnitDFXYZ[0] = -DFXYZ[0] / sqrt(DOT(DFXYZ, DFXYZ)); // unit surface normals
		UnitDFXYZ[1] = -DFXYZ[1] / sqrt(DOT(DFXYZ, DFXYZ));
		UnitDFXYZ[2] = -DFXYZ[2] / sqrt(DOT(DFXYZ, DFXYZ));
		IncidentAngle = acos(DOT(CosKLM, UnitDFXYZ));
		Rs = sqr(((Refr1 * cos(IncidentAngle) - Refr2 * sqrt(1 - sqr(Refr1 * sin(IncidentAngle) / Refr2)))) /
				 ((Refr1 * cos(IncidentAngle) + Refr2 * sqrt(1 - sqr(Refr1 * sin(IncidentAngle) / Refr2)))));
		Rp = sqr(((Refr1 * sqrt(1 - sqr(Refr1 * sin(IncidentAngle) / Refr2))) - Refr2 * cos(IncidentAngle)) /
				 ((Refr1 * sqrt(1 - sqr(Refr1 * sin(IncidentAngle) / Refr2))) + Refr2 * cos(IncidentAngle)));
		Rave = (Rp + Rs) / 2.0; // average of s and p polarized light; equal parts of both = non-polarized
		if (Rave < myrng())		// transmitted through surface
		{
			Gamn = -B / (2.0 * A);

			// Begin Newton-Raphson loop to converge on correct root.
			bool converged = false;
			for (i = 1; i < NIter; i++)
			{
				Gamn1 = (Gamn * Gamn - B) / (2.0 * (Gamn + A));
				if (fabs(Gamn - Gamn1) < Epsilon)
				{
					converged = true;
					break;
				}

				Gamn = Gamn1;
			}
			// Failed to converge
			if (converged == false)
			{
				*ErrorFlag = 12;
				return;
			}

			// Have converged on Gamma, Compute direction cosines of refracted ray.
			// Label_Converge:
			for (i = 0; i < 3; i++)
				CosOut[i] = RMU * CosKLM[i] + Gamn1 * DFXYZ[i];
		}
		else // reflected from surface
		{
			A = DOT(CosKLM, DFXYZ) / DOT(DFXYZ, DFXYZ);
			for (i = 0; i < 3; i++)
				CosOut[i] = CosKLM[i] - 2.0 * A * DFXYZ[i];
		}
		return;
		break;
	}

		/*{  InteractionType = 2, Reflection
		===============================================================================}*/
	case 2:
	{
		A = DOT(CosKLM, DFXYZ) / DOT(DFXYZ, DFXYZ);
		// Compute direction cosines for reflected ray
		for (i = 0; i < 3; i++)
			CosOut[i] = CosKLM[i] - 2.0 * A * DFXYZ[i];

		return;
		break;
	}

	// 	/*{  InteractionType = 3, Aperture Stop
	// 	===============================================================================}*/
	// case 3:
	// {
	// 	X = PosXYZ[0];
	// 	Y = PosXYZ[1];
	// 	IType = Opticl->ApertureStopOrGratingType;
	// 	A1 = Opticl->AB12[0];
	// 	B1 = Opticl->AB12[1];

	// 	bool ray_missed_aperture = false;
	// 	if (IType == 1) // Slit Aperture
	// 	{
	// 		A2 = Opticl->AB12[2];
	// 		B2 = Opticl->AB12[3];
	// 		if (X < A1 || X > A2)
	// 		{
	// 			*ErrorFlag = 31;
	// 			ray_missed_aperture = true;
	// 		}
	// 		else
	// 		{
	// 			if (Y >= B1 && Y <= B2)
	// 				return;

	// 			*ErrorFlag = 31;
	// 			ray_missed_aperture = true;
	// 		}
	// 	}

	// 	else if (IType == 2) // Elliptical Aperture
	// 	{
	// 		Ellips = X * X / (A1 * A1) + Y * Y / (B1 * B1);
	// 		if (Ellips <= 1.0)
	// 			return;
	// 		*ErrorFlag = 32;
	// 	}

	// 	// RayMissesAperture:
	// 	// Ray misses aperture
	// 	if (ray_missed_aperture == true)
	// 	{
	// 		for (i = 0; i < 3; i++)
	// 			CosOut[i] = 0.0;
	// 	}
	// 	return;

	// 	break;
	// }

	// 	/*{  InteractionType = 4,5; Diffraction
	// 	===============================================================================}*/
	// case 4:
	// case 5:
	// {
	// 	IType = Opticl->ApertureStopOrGratingType;
	// 	NMord = Opticl->DiffractionOrder;
	// 	Refr1 = Opticl->RefractiveIndex[0];
	// 	Refr2 = Opticl->RefractiveIndex[2];
	// 	RMU = Refr1 / Refr2;
	// 	D2 = DOT(DFXYZ, DFXYZ);
	// 	RK = DFXYZ[0];
	// 	RL = DFXYZ[1];
	// 	RM = DFXYZ[2];
	// 	X = PosXYZ[0];
	// 	Y = PosXYZ[1];

	// 	if (IType == 1) // Parallel plane grating
	// 	{
	// 		Denom = RL * RL + RM * RM;
	// 		U = 1.0 / sqrt(1.0 + RK * RK / Denom);
	// 		V = -RK * RL * U / Denom;
	// 		W = -RK * RM * U / Denom;
	// 		Varr = X;
	// 		GFactr = 1.0 / U;
	// 	}

	// 	else if (IType == 2) // Concentric Cylinder Grating
	// 	{
	// 		Rho2 = X * X + Y * Y;
	// 		Rho = sqrt(Rho2);
	// 		RM2 = RM * RM;
	// 		Term = RL * X - RK * Y;
	// 		G = sqrt(D2 * (RM2 * Rho2 + Term * Term));
	// 		U = (RM2 * X + RL * Term) / G;
	// 		V = (RM2 * Y - RK * Term) / G;
	// 		W = -RM * (RK * X + RL * Y) / G;
	// 		Varr = Rho;
	// 		GFactr = Rho / (X * U + Y * V);
	// 	}
	// 	// CompDiffInt:         //Compute interaction due to diffraction
	// 	CosUVW[0] = U;
	// 	CosUVW[1] = V;
	// 	CosUVW[2] = W;

	// 	D = 0.0;
	// 	XX = 1.0;

	// 	for (i = 0; i < 4; i++)
	// 	{
	// 		D = D + Opticl->AB12[i] * XX;
	// 		XX = XX * Varr;
	// 	}

	// 	D = D * GFactr;
	// 	Ordiff = NMord;
	// 	RLamda = Ordiff * Wavelength / (Refr2 * D);
	// 	B = (RMU * RMU - 1.0 + RLamda * RLamda - 2.0 * RMU * RLamda * DOT(CosKLM, CosUVW)) / D2;
	// 	A = RMU * DOT(CosKLM, DFXYZ) / D2;
	// 	A2 = A * A;
	// 	if (B > A2) // Total internal reflection
	// 	{
	// 		for (i = 0; i < 3; i++)
	// 			CosOut[i] = 0.0;
	// 		*ErrorFlag = 11;
	// 		return;
	// 	}

	// 	Gamn = -B / (2.0 * A);
	// 	if (InteractionType == 5)
	// 		Gamn = -Gamn - 2.0 * A;

	// 	// Begin Newton-Raphson loop to converge on correct root.
	// 	i = 0;
	// 	bool converged = false;
	// 	while (i++ < NIter)
	// 	{
	// 		Gamn1 = (Gamn * Gamn - B) / (2.0 * (Gamn + A));
	// 		if (fabs(Gamn - Gamn1) < Epsilon)
	// 		{
	// 			converged = true;
	// 			break;
	// 		}
	// 		Gamn = Gamn1;
	// 	}
	// 	// Failed to converge
	// 	if (converged == false)
	// 	{
	// 		*ErrorFlag = 12;
	// 		return;
	// 	}
	// 	// Have converged on Gamn1. Compute direction cosines of diffracted ray.
	// 	// CompDCos:
	// 	for (i = 0; i < 3; i++)
	// 		CosOut[i] = RMU * CosKLM[i] - RLamda * CosUVW[i] + Gamn1 * DFXYZ[i];

	// 	break;
	// }
	default:
		break;
	}
	return;
}

bool SunToPrimaryStage(
	TSystem *System,
	TStage *Stage,
	TSun *Sun,
	double PosSunStage[3])
{

	/*{Purpose: To compute the sun position within primary sage and the maximum radius of a cicle seen from sun which encircles
	all elements  within the primary stage.  Used for genenating rays from sun.   modified on 09/26/05 to establish rectangular
	region of interest - more efficient.
		 Input - Sys     = Primary stage
				 Sun       = Sun description block
		 Output - PosSunStage  = position of sun in primary stage coordinate system
				  Sun.MaxRad =    maximum radius of a cicle seen from sun which encircles
							  all elements within the primary stage  relative to center of mass of all elements in that stage
				  Sun.Xcm
				  Sun.Ycm    =  center of mass of all elements in primary stage as seen in sun coordinate system
	}*/

	double dx = 0, dy = 0, dz = 0, dtot = 0;
	double CosSunGlob[3] = {0.0, 0.0, 0.0};
	double PosSunGlob[3] = {0.0, 0.0, 0.0};
	double CosSunStage[3] = {0.0, 0.0, 0.0};

	uint_fast64_t i = 0;
	double x = 0, y = 0, radius = 0;
	double Origin[3] = {0.0, 0.0, 0.0};
	double CosDum[3] = {0.0, 0.0, 0.0};
	double PosLoc[3] = {0.0, 0.0, 0.0};
	double CosLoc[3] = {0.0, 0.0, 0.0};
	double RRefToLoc[3][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
	double radius1 = 0.0, radius2 = 0.0, radius3 = 0.0, radius4 = 0.0, radiustemp = 0.0;
	double Xsum = 0.0, Ysum = 0.0, xminsun = 0.0, yminsun = 0.0, xmaxsun = 0.0, ymaxsun = 0.0;
	double XLegofRadius = 0.0;

	// PosSunGlob[0] = Sun.Origin[0];//Position of sun coord. system origin in global system
	// PosSunGlob[1] = Sun.Origin[1]; //changed 5/1/00 to place sun at primary stage origin; direction vector
	// PosSunGlob[2] = Sun.Origin[2]; //calculated below from difference between entered sun position and global
	PosSunGlob[0] = Stage->Origin[0]; // origin
	PosSunGlob[1] = Stage->Origin[1];
	PosSunGlob[2] = Stage->Origin[2];

	// First calculate direction cosines of sun z-axis in global coord. system
	dx = 0.0 - Sun->Origin[0]; // changed 5/1/00 to tie the sun direction to global coordinate system origin
	dy = 0.0 - Sun->Origin[1]; // for any stage; not different for each stage.  this represents reality because
	dz = 0.0 - Sun->Origin[2]; // sun is essentially inifinitely far away.
	dtot = sqrt(dx * dx + dy * dy + dz * dz);

	if (dtot == 0.0)
	{
		// flag error somehow?
		System->errlog("error calculating sun position in primary stage, dtot = 0.0\n");
		return false;
	}

	dx = dx / dtot; // unit vector in global coord.system of sun coord. system z axis.
	dy = dy / dtot;
	dz = dz / dtot;

	CosSunGlob[0] = dx; // direction cosines of sun Z-axis in global system.
	CosSunGlob[1] = dy;
	CosSunGlob[2] = dz;

	// Transform sun direction vector to Stage system; CosSunStage is dir cosines of sun ray in Stage coord. system
	// PosSunStage is position of sun coord. system origin in Stage system
	TransformToLocal(PosSunGlob, CosSunGlob, Stage->Origin, Stage->RRefToLoc, PosSunStage, CosSunStage);

	Sun->Euler[0] = atan2(CosSunStage[0], CosSunStage[2]); // Euler angles relating sun to Stage system
	Sun->Euler[1] = asin(CosSunStage[1]);
	Sun->Euler[2] = 0.0;

	/*     {Now we have the Euler angles from Stage to the sun coordinate system.  We have to now transform the
		  element locations in the stage system to the sun coordinate system and find the smallest circle in the
		  xy plane of the sun system that completely encompasses the projected images of the elements onto that plane}*/

	Origin[0] = 0.0; // Origin of transformed system and stage system the same
	Origin[1] = 0.0;
	Origin[2] = 0.0;

	CosDum[0] = 0.0; // direction cosines not important; only interested in point locations
	CosDum[1] = 0.0;
	CosDum[2] = 1.0;

	Sun->MaxRad = 0.0;
	Sun->Xcm = 0.0;
	Sun->Ycm = 0.0;
	Sun->MaxXSun = -1.0e20;
	Sun->MinXSun = 1.0e20;
	Sun->MaxYSun = -1.0e20;
	Sun->MinYSun = 1.0e20;

	CalculateTransformMatrices(Sun->Euler, RRefToLoc, Sun->RLocToRef);

	//{Now calculate center of mass of projected distribution. Added 09/26/05}
	Xsum = 0.0;
	Ysum = 0.0;
	for (i = 0; i < Stage->ElementList.size(); i++)
	{
		// if (!Stage->ElementList[i]->Enabled)
		// 	continue;
		TransformToLocal(Stage->ElementList[i]->Origin, CosDum, Origin, RRefToLoc, PosLoc, CosLoc);
		// Now have PosLoc which is the projected position of element[i] in xy plane of sun coord. system
		Xsum = Xsum + PosLoc[0];
		Ysum = Ysum + PosLoc[1];
	}
	Sun->Xcm = Xsum / Stage->ElementList.size(); // center of mass of distribution of element locations as projected in sun coord.
	Sun->Ycm = Ysum / Stage->ElementList.size(); // system.   Added 09/26/05

	size_t nelements = 0;

	for (i = 0; i < Stage->ElementList.size(); i++)
	{
		// if (!Stage->ElementList[i]->Enabled)
		// 	continue;

		TransformToLocal(Stage->ElementList[i]->Origin, CosDum, Origin, RRefToLoc, PosLoc, CosLoc);
		// Now have PosLoc which is the projected position of element[i] in xy plane of sun coord. system
		x = PosLoc[0] - Sun->Xcm; // changes origin to center of mass of all elements  09/26/05
		y = PosLoc[1] - Sun->Ycm;
		radius = sqrt(x * x + y * y);

		xminsun = PosLoc[0];
		xmaxsun = PosLoc[0];
		yminsun = PosLoc[1];
		ymaxsun = PosLoc[1];

		// save the projected position of the element on the sun coordinate plane
		Stage->ElementList[i]->PosSunCoords[0] = PosLoc[0];
		Stage->ElementList[i]->PosSunCoords[1] = PosLoc[1];
		Stage->ElementList[i]->PosSunCoords[2] = PosLoc[2];

		// Add radius of element circle of interest - different radius for each shape: circular, hexagonal, rectangular, triangular, annular, off-axis rectangle
		if (Stage->ElementList[i]->ShapeIndex == 'c' || Stage->ElementList[i]->ShapeIndex == 'C')
		{
			radius = radius + Stage->ElementList[i]->ParameterA / 2.0;
			xminsun = xminsun - Stage->ElementList[i]->ParameterA / 2.0;
			yminsun = yminsun - Stage->ElementList[i]->ParameterA / 2.0;
			xmaxsun = xmaxsun + Stage->ElementList[i]->ParameterA / 2.0;
			ymaxsun = ymaxsun + Stage->ElementList[i]->ParameterA / 2.0;
		}
		if (Stage->ElementList[i]->ShapeIndex == 'h' || Stage->ElementList[i]->ShapeIndex == 'H')
		{
			radius = radius + Stage->ElementList[i]->ParameterA / 2.0;
			xminsun = xminsun - Stage->ElementList[i]->ParameterA / 2.0;
			yminsun = yminsun - Stage->ElementList[i]->ParameterA / 2.0;
			xmaxsun = xmaxsun + Stage->ElementList[i]->ParameterA / 2.0;
			ymaxsun = ymaxsun + Stage->ElementList[i]->ParameterA / 2.0;
		}
		if (Stage->ElementList[i]->ShapeIndex == 'r' || Stage->ElementList[i]->ShapeIndex == 'R')
		{
			radius = radius + sqrt((sqr(Stage->ElementList[i]->ParameterA) + sqr(Stage->ElementList[i]->ParameterB)) / 4.0);
			xminsun = xminsun - sqrt((sqr(Stage->ElementList[i]->ParameterA) + sqr(Stage->ElementList[i]->ParameterB)) / 4.0);
			yminsun = yminsun - sqrt((sqr(Stage->ElementList[i]->ParameterA) + sqr(Stage->ElementList[i]->ParameterB)) / 4.0);
			xmaxsun = xmaxsun + sqrt((sqr(Stage->ElementList[i]->ParameterA) + sqr(Stage->ElementList[i]->ParameterB)) / 4.0);
			ymaxsun = ymaxsun + sqrt((sqr(Stage->ElementList[i]->ParameterA) + sqr(Stage->ElementList[i]->ParameterB)) / 4.0);
		}
		if (Stage->ElementList[i]->ShapeIndex == 't' || Stage->ElementList[i]->ShapeIndex == 'T')
		{
			radius = radius + Stage->ElementList[i]->ParameterA / (2.0 * cos(30.0 * (ACOSM1O180)));
			xminsun = xminsun - Stage->ElementList[i]->ParameterA / (2.0 * cos(30.0 * (ACOSM1O180)));
			yminsun = yminsun - Stage->ElementList[i]->ParameterA / (2.0 * cos(30.0 * (ACOSM1O180)));
			xmaxsun = xmaxsun + Stage->ElementList[i]->ParameterA / (2.0 * cos(30.0 * (ACOSM1O180)));
			ymaxsun = ymaxsun + Stage->ElementList[i]->ParameterA / (2.0 * cos(30.0 * (ACOSM1O180)));
		}
		if (Stage->ElementList[i]->ShapeIndex == 'a' || Stage->ElementList[i]->ShapeIndex == 'A')
		{
			radius = radius + Stage->ElementList[i]->ParameterB;
			xminsun = xminsun - Stage->ElementList[i]->ParameterB;
			yminsun = yminsun - Stage->ElementList[i]->ParameterB;
			xmaxsun = xmaxsun + Stage->ElementList[i]->ParameterB;
			ymaxsun = ymaxsun + Stage->ElementList[i]->ParameterB;
		}

		if (Stage->ElementList[i]->ShapeIndex == 'l' || Stage->ElementList[i]->ShapeIndex == 'L')
		{
			if (fabs(Stage->ElementList[i]->ParameterB) >= fabs(Stage->ElementList[i]->ParameterA)) // change made on 02-12-09  replaced above with following block
				XLegofRadius = Stage->ElementList[i]->ParameterB;
			else
				XLegofRadius = Stage->ElementList[i]->ParameterA;

			radius = radius + sqrt(sqr(XLegofRadius) + 0.25 * Stage->ElementList[i]->ParameterC * Stage->ElementList[i]->ParameterC);
			xminsun = xminsun - sqrt(sqr(XLegofRadius) + 0.25 * Stage->ElementList[i]->ParameterC * Stage->ElementList[i]->ParameterC);
			yminsun = yminsun - sqrt(sqr(XLegofRadius) + 0.25 * Stage->ElementList[i]->ParameterC * Stage->ElementList[i]->ParameterC);
			xmaxsun = xmaxsun + sqrt(sqr(XLegofRadius) + 0.25 * Stage->ElementList[i]->ParameterC * Stage->ElementList[i]->ParameterC);
			ymaxsun = ymaxsun + sqrt(sqr(XLegofRadius) + 0.25 * Stage->ElementList[i]->ParameterC * Stage->ElementList[i]->ParameterC);
		}

		//****************************************************************
		if (Stage->ElementList[i]->ShapeIndex == 'i' || Stage->ElementList[i]->ShapeIndex == 'I')
		{
			radius1 = sqrt(sqr(Stage->ElementList[i]->ParameterA) + sqr(Stage->ElementList[i]->ParameterB));
			radius2 = sqrt(sqr(Stage->ElementList[i]->ParameterC) + sqr(Stage->ElementList[i]->ParameterD));
			radius3 = sqrt(sqr(Stage->ElementList[i]->ParameterE) + sqr(Stage->ElementList[i]->ParameterF));
			radiustemp = radius1;
			if (radius2 > radiustemp)
				radiustemp = radius2;
			if (radius3 > radiustemp)
				radiustemp = radius3;
			radius = radius + radiustemp;
			xminsun = xminsun - radiustemp;
			yminsun = yminsun - radiustemp;
			xmaxsun = xmaxsun + radiustemp;
			ymaxsun = ymaxsun + radiustemp;
		}
		if (Stage->ElementList[i]->ShapeIndex == 'q' || Stage->ElementList[i]->ShapeIndex == 'Q')
		{
			radius1 = sqrt(sqr(Stage->ElementList[i]->ParameterA) + sqr(Stage->ElementList[i]->ParameterB));
			radius2 = sqrt(sqr(Stage->ElementList[i]->ParameterC) + sqr(Stage->ElementList[i]->ParameterD));
			radius3 = sqrt(sqr(Stage->ElementList[i]->ParameterE) + sqr(Stage->ElementList[i]->ParameterF));
			radius4 = sqrt(sqr(Stage->ElementList[i]->ParameterG) + sqr(Stage->ElementList[i]->ParameterH));
			radiustemp = radius1;
			if (radius2 > radiustemp)
				radiustemp = radius2;
			if (radius3 > radiustemp)
				radiustemp = radius3;
			if (radius4 > radiustemp)
				radiustemp = radius4;
			radius = radius + radiustemp;
			xminsun = xminsun - radiustemp;
			yminsun = yminsun - radiustemp;
			xmaxsun = xmaxsun + radiustemp;
			ymaxsun = ymaxsun + radiustemp;
		}

		if (radius > Sun->MaxRad)
			Sun->MaxRad = radius; // establishes a circular region

		if (xminsun < Sun->MinXSun)
			Sun->MinXSun = xminsun; // restablishes a rectangular region instead of a circular region
		if (xmaxsun > Sun->MaxXSun)
			Sun->MaxXSun = xmaxsun; // Added 09/26/05
		if (yminsun < Sun->MinYSun)
			Sun->MinYSun = yminsun;
		if (ymaxsun > Sun->MaxYSun)
			Sun->MaxYSun = ymaxsun;

		nelements++;
	}

	if (nelements == 0)
		System->errlog("error calculating sun position in primary stage because no elements were enabled");

	return (nelements > 0);
}

void SurfaceNormalErrors(MTRand &myrng, double CosIn[3],
						 TOpticalProperties *OptProperties,
						 double CosOut[3]) noexcept(false) // throw(nanexcept)
{

	/*{Purpose:  To add error terms to the surface normal vector at the surface in question

			   Input - Seed    = Seed for RNG
					   CosIn   = Direction cosine vector of surface normal to which errors will be applied.
					   Element = Element data record
					   DFXYZ   = surface normal vector at interaction point

			   Output - CosOut  = Output direction cosine vector of surface normal after error terms have been included
					   }*/

	int i = 0;
	double Origin[3] = {0.0, 0.0, 0.0},
		   Euler[3] = {0.0, 0.0, 0.0};
	double PosIn[3] = {0.0, 0.0, 0.0},
		   PosOut[3] = {0.0, 0.0, 0.0};
	char dist = ' ';
	double delop = 0.0, delop3 = 0.0, thetax = 0.0,
		   thetay = 0.0, ttheta = 0.0, theta2 = 0.0,
		   phi = 0.0, theta = 0.0;
	double RRefToLoc[3][3] = {{0.0, 0.0, 0.0},
							  {0.0, 0.0, 0.0},
							  {0.0, 0.0, 0.0}};
	double RLocToRef[3][3] = {{0.0, 0.0, 0.0},
							  {0.0, 0.0, 0.0},
							  {0.0, 0.0, 0.0}};

	if (CosIn[2] == 0.0)
	{
		if (CosIn[0] == 0.0)
		{
			Euler[0] = 0.0;
			Euler[1] = M_PI / 2.0;
			goto Label_9;
		}
		else
		{
			Euler[0] = M_PI / 2.0;
			goto Label_8;
		}
	}

	Euler[0] = atan2(CosIn[0], CosIn[2]);
Label_8:
	Euler[1] = atan2(CosIn[1], sqrt(CosIn[0] * CosIn[0] + CosIn[2] * CosIn[2]));
Label_9:
	Euler[2] = 0.0;

	CalculateTransformMatrices(Euler, RRefToLoc, RLocToRef);

	dist = OptProperties->DistributionType;
	delop = OptProperties->RMSSlopeError / 1000.0;

	int nninner = 0;
	switch (dist)
	{
	case 'g':
	case 'G':
		// gaussian distribution
		thetax = myrng.randNorm(0., delop);
		thetay = myrng.randNorm(0., delop);

		theta2 = thetax * thetax + thetay * thetay;

		break;

	case 'p':
	case 'P':
		// pillbox distribution
		do
		{
			thetax = 2.0 * delop * myrng() - delop;
			thetay = 2.0 * delop * myrng() - delop;
			theta2 = thetax * thetax + thetay * thetay;
		} while (theta2 > (delop * delop));

		break;
	}

	/* {Transform to local coordinate system of ray to set up rotation matrices for coord and inverse
	   transforms} */

	TransformToLocal(PosIn, CosIn, Origin, RRefToLoc, PosOut, CosOut);

	/* {Generate errors in terms of direction cosines in local ray coordinate system} */
	theta = sqrt(theta2);
	// phi = atan2(thetay, thetax); //This function appears to  present irregularities that bias results incorrectly for small values of thetay or thetax
	phi = myrng() * 2.0 * 3.1415926535897932385; // Therefore have chosen to randomize phi rather than calculate from randomized theta components
												 //  obtained from the distribution. The two approaches are equivalent save for this issue with
												 //  arctan2.      wendelin 01-12-11

	CosOut[0] = sin(theta) * cos(phi);
	CosOut[1] = sin(theta) * sin(phi);
	CosOut[2] = cos(theta);

	for (i = 0; i < 3; i++)
	{
		PosIn[i] = PosOut[i];
		CosIn[i] = CosOut[i];
	}

	/*{Transform perturbed ray back to element system}*/
	TransformToReference(PosIn, CosIn, Origin, RLocToRef, PosOut, CosOut);
}

void DetermineElementIntersectionNew(
	TElement *Element,
	double PosRayIn[3],
	double CosRayIn[3],
	double PosRayOut[3],
	double CosRayOut[3],
	double DFXYZ[3],
	double *PathLength,
	int *ErrorFlag,
	int *Intercept,
	int *BacksideFlag)
{
	double r = 0.0, Ro = 0.0, Ri = 0.0, XL = 0.0, x = 0.0, y = 0.0;
	double /*SLOP30 = 0.0, SLOP60 = 0.0,*/ Y1 = 0.0, Y2 = 0.0, Y3 = 0.0, Y4 = 0.0;
	double P1x = 0.0, P1y = 0.0, P2x = 0.0, P2y = 0.0, P3x = 0.0, P3y = 0.0, P4x = 0.0, P4y = 0.0;
	// double Tn;
	int in_quad = 0;
	// ZAperPlane: real;

	*ErrorFlag = 0;
	double SLOP30 = 0.57735026918962573; // tan(30.0*(acos(-1.0)/180.0));
	double SLOP60 = 1.7320508075688767;	 // tan(60.0*(acos(-1.0)/180.0));

	// AperturePlane(Element);           <------- calculated now in ODConcentrator
	// ZAperPlane = Element->ZAperture;

	// find intersection with surface first
	Intersect(PosRayIn, CosRayIn, Element, PosRayOut, CosRayOut, DFXYZ, PathLength, ErrorFlag);
	if (*ErrorFlag > 0 || *PathLength < 0)
	{
		*Intercept = 0;
		PosRayOut[0] = 0.0;
		PosRayOut[1] = 0.0;
		PosRayOut[2] = 0.0;
		CosRayOut[0] = 0.0;
		CosRayOut[1] = 0.0;
		CosRayOut[2] = 0.0;
		DFXYZ[0] = 0.0;
		DFXYZ[1] = 0.0;
		DFXYZ[2] = 0.0;
		*BacksideFlag = 0;
		*PathLength = 0.0;
		// goto Label_100;
		return;
	}

	x = PosRayOut[0];
	y = PosRayOut[1];
	r = sqrt(x * x + y * y);

	switch (Element->ShapeIndex)
	{
	case 'c':
	case 'C': // circular aperture
		Ro = Element->ParameterA / 2.0;

		if (r > Ro) // ray falls outsideside circular aperture
		{
			*Intercept = 0;
			PosRayOut[0] = 0.0;
			PosRayOut[1] = 0.0;
			PosRayOut[2] = 0.0;
			CosRayOut[0] = 0.0;
			CosRayOut[1] = 0.0;
			CosRayOut[2] = 0.0;
			DFXYZ[0] = 0.0;
			DFXYZ[1] = 0.0;
			DFXYZ[2] = 0.0;
			*PathLength = 0.0;
			*ErrorFlag = 0;
			*BacksideFlag = 0;
			// goto Label_100;
			break;
		}
		else
		{
			if (DOT(CosRayIn, DFXYZ) < 0)
				*BacksideFlag = 0;
			else
				*BacksideFlag = 1;
			*Intercept = 1;
			// goto Label_100;
			break;
		}
		break;

	case 'h':
	case 'H': // hexagonal aperture
		Ro = Element->ParameterA / 2.0;

		if (r > Ro) // ray falls outside circular circumference aperture
		{
			*Intercept = 0;
			PosRayOut[0] = 0.0;
			PosRayOut[1] = 0.0;
			PosRayOut[2] = 0.0;
			CosRayOut[0] = 0.0;
			CosRayOut[1] = 0.0;
			CosRayOut[2] = 0.0;
			DFXYZ[0] = 0.0;
			DFXYZ[1] = 0.0;
			DFXYZ[2] = 0.0;
			*PathLength = 0.0;
			*ErrorFlag = 0;
			*BacksideFlag = 0;
			// goto Label_100;
			break;
		}
		Ri = Ro * cos(30.0 * (ACOSM1O180));

		if (r <= Ri) // ray falls inside inscribed circle
		{
			if (DOT(CosRayIn, DFXYZ) < 0)
				*BacksideFlag = 0;
			else
				*BacksideFlag = 1;
			*Intercept = 1;
			// goto Label_100;
			break;
		}

		XL = sqrt(Ro * Ro - Ri * Ri); // otherwise break hexagon into 3 sections
		if ((x <= Ro) && (x > XL))	  // 1st section
		{
			Y1 = SLOP60 * (x - Ro);
			Y2 = -Y1;
			if ((y >= Y1) && (y <= Y2))
			{
				if (DOT(CosRayIn, DFXYZ) < 0)
					*BacksideFlag = 0;
				else
					*BacksideFlag = 1;

				*Intercept = 1;
				// goto Label_100;
				break;
			}
			*Intercept = 0;
			PosRayOut[0] = 0.0;
			PosRayOut[1] = 0.0;
			PosRayOut[2] = 0.0;
			CosRayOut[0] = 0.0;
			CosRayOut[1] = 0.0;
			CosRayOut[2] = 0.0;
			DFXYZ[0] = 0.0;
			DFXYZ[1] = 0.0;
			DFXYZ[2] = 0.0;
			*PathLength = 0.0;
			*BacksideFlag = 0;
			*ErrorFlag = 0;
			// goto Label_100;
			break;
		}

		if ((x <= XL) && (x >= -XL)) // 2nd section
		{
			if ((y >= -Ri) && (y <= Ri))
			{
				if (DOT(CosRayIn, DFXYZ) < 0)
					*BacksideFlag = 0;
				else
					*BacksideFlag = 1;
				*Intercept = 1;
				// goto Label_100;
				break;
			}
			*Intercept = 0;
			PosRayOut[0] = 0.0;
			PosRayOut[1] = 0.0;
			PosRayOut[2] = 0.0;
			CosRayOut[0] = 0.0;
			CosRayOut[1] = 0.0;
			CosRayOut[2] = 0.0;
			DFXYZ[0] = 0.0;
			DFXYZ[1] = 0.0;
			DFXYZ[2] = 0.0;
			*PathLength = 0.0;
			*BacksideFlag = 0;
			*ErrorFlag = 0;
			// goto Label_100;
			break;
		}

		if ((x < -XL) && (x >= -Ro)) // 3rd section
		{
			Y3 = SLOP60 * (x + Ro);
			Y4 = -Y3;
			if ((y >= Y4) && (y <= Y3))
			{
				if (DOT(CosRayIn, DFXYZ) < 0)
					*BacksideFlag = 0;
				else
					*BacksideFlag = 1;
				*Intercept = 1;
				// goto Label_100;
				break;
			}
			*Intercept = 0;
			PosRayOut[0] = 0.0;
			PosRayOut[1] = 0.0;
			PosRayOut[2] = 0.0;
			CosRayOut[0] = 0.0;
			CosRayOut[1] = 0.0;
			CosRayOut[2] = 0.0;
			DFXYZ[0] = 0.0;
			DFXYZ[1] = 0.0;
			DFXYZ[2] = 0.0;
			*PathLength = 0.0;
			*BacksideFlag = 0;
			*ErrorFlag = 0;
			// goto Label_100;
			break;
		}
		break;

	case 't':
	case 'T': // Triangular aperture
		Ro = Element->ParameterA / 2.0;

		if (r > Ro) // ray falls outside circular circumference aperture
		{
			*Intercept = 0;
			PosRayOut[0] = 0.0;
			PosRayOut[1] = 0.0;
			PosRayOut[2] = 0.0;
			CosRayOut[0] = 0.0;
			CosRayOut[1] = 0.0;
			CosRayOut[2] = 0.0;
			DFXYZ[0] = 0.0;
			DFXYZ[1] = 0.0;
			DFXYZ[2] = 0.0;
			*PathLength = 0.0;
			*BacksideFlag = 0;
			*ErrorFlag = 0;
			// goto Label_100;
			break;
		}
		Ri = Ro * sin(30.0 * (ACOSM1O180));

		if (r <= Ri) // ray falls inside inscribed circle
		{
			if (DOT(CosRayIn, DFXYZ) < 0)
				*BacksideFlag = 0;
			else
				*BacksideFlag = 1;
			*Intercept = 1;
			// goto Label_100;
			break;
		}

		if ((x <= Ro) && (x > 0.0)) // 1st section
		{
			Y1 = -SLOP60 * (x - Ri / cos(30.0 * (ACOSM1O180)));
			Y2 = -Ri;
			if ((y <= Y1) && (y >= Y2))
			{
				if (DOT(CosRayIn, DFXYZ) < 0)
					*BacksideFlag = 0;
				else
					*BacksideFlag = 1;
				*Intercept = 1;
				// goto Label_100;
				break;
			}
			*Intercept = 0;
			PosRayOut[0] = 0.0;
			PosRayOut[1] = 0.0;
			PosRayOut[2] = 0.0;
			CosRayOut[0] = 0.0;
			CosRayOut[1] = 0.0;
			CosRayOut[2] = 0.0;
			DFXYZ[0] = 0.0;
			DFXYZ[1] = 0.0;
			DFXYZ[2] = 0.0;
			*PathLength = 0.0;
			*BacksideFlag = 0;
			*ErrorFlag = 0;
			// goto Label_100;
			break;
		}
		if ((x >= -Ro) && (x <= 0.0)) // 2nd section
		{
			Y3 = SLOP60 * (x + Ri / cos(30.0 * (ACOSM1O180)));
			Y4 = -Ri;
			if ((y >= Y4) && (y <= Y3))
			{
				if (DOT(CosRayIn, DFXYZ) < 0)
					*BacksideFlag = 0;
				else
					*BacksideFlag = 1;
				*Intercept = 1;
				// goto Label_100;
				break;
			}
			*Intercept = 0;
			PosRayOut[0] = 0.0;
			PosRayOut[1] = 0.0;
			PosRayOut[2] = 0.0;
			CosRayOut[0] = 0.0;
			CosRayOut[1] = 0.0;
			CosRayOut[2] = 0.0;
			DFXYZ[0] = 0.0;
			DFXYZ[1] = 0.0;
			DFXYZ[2] = 0.0;
			*PathLength = 0.0;
			*BacksideFlag = 0;
			*ErrorFlag = 0;
			// goto Label_100;
			break;
		}
		break;

	case 'r':
	case 'R': // Rectangular aperture

		if ((x > Element->ParameterA / 2.0) || (x < -Element->ParameterA / 2.0))
		{
			*Intercept = 0;
			PosRayOut[0] = 0.0;
			PosRayOut[1] = 0.0;
			PosRayOut[2] = 0.0;
			CosRayOut[0] = 0.0;
			CosRayOut[1] = 0.0;
			CosRayOut[2] = 0.0;
			DFXYZ[0] = 0.0;
			DFXYZ[1] = 0.0;
			DFXYZ[2] = 0.0;
			*PathLength = 0.0;
			*BacksideFlag = 0;
			*ErrorFlag = 0;
			// goto Label_100;
			break;
		}

		if ((y > Element->ParameterB / 2.0) || (y < -Element->ParameterB / 2.0))
		{
			*Intercept = 0;
			PosRayOut[0] = 0.0;
			PosRayOut[1] = 0.0;
			PosRayOut[2] = 0.0;
			CosRayOut[0] = 0.0;
			CosRayOut[1] = 0.0;
			CosRayOut[2] = 0.0;
			DFXYZ[0] = 0.0;
			DFXYZ[1] = 0.0;
			DFXYZ[2] = 0.0;
			*PathLength = 0.0;
			*BacksideFlag = 0;
			*ErrorFlag = 0;
			// goto Label_100;
			break;
		}

		if (DOT(CosRayIn, DFXYZ) < 0) // successfully falls on rectangle
			*BacksideFlag = 0;
		else
			*BacksideFlag = 1;

		*Intercept = 1;
		// goto Label_100;

		break;

	case 'a':
	case 'A': // Annulus or torus contour

		if ((Element->ParameterA == 0.0) && (Element->ParameterB == 0.0))
		{
			// Naturally goes to Label_5
			// goto Label_5; //torus
		}
		else if ((r < Element->ParameterA) || (r > Element->ParameterB))
		{
			*Intercept = 0;
			PosRayOut[0] = 0.0;
			PosRayOut[1] = 0.0;
			PosRayOut[2] = 0.0;
			CosRayOut[0] = 0.0;
			CosRayOut[1] = 0.0;
			CosRayOut[2] = 0.0;
			DFXYZ[0] = 0.0;
			DFXYZ[1] = 0.0;
			DFXYZ[2] = 0.0;
			*PathLength = 0.0;
			*BacksideFlag = 0;
			*ErrorFlag = 0;
			// goto Label_100;
			break;
		}
		// Label_5:
		if (x >= 0.0)
		{
			if ((asin(y / r) > Element->ParameterC * (ACOSM1O180) / 2.0) || (asin(y / r) < -Element->ParameterC * (ACOSM1O180) / 2.0))
			{
				*Intercept = 0;
				PosRayOut[0] = 0.0;
				PosRayOut[1] = 0.0;
				PosRayOut[2] = 0.0;
				CosRayOut[0] = 0.0;
				CosRayOut[1] = 0.0;
				CosRayOut[2] = 0.0;
				DFXYZ[0] = 0.0;
				DFXYZ[1] = 0.0;
				DFXYZ[2] = 0.0;
				*PathLength = 0.0;
				*BacksideFlag = 0;
				*ErrorFlag = 0;
				// goto Label_100;
				break;
			}

			if (DOT(CosRayIn, DFXYZ) < 0) // successfully falls on annular section
				*BacksideFlag = 0;
			else
				*BacksideFlag = 1;
			*Intercept = 1;
			// goto Label_100;
			break;
		}

		if (x < 0.0)
		{
			if ((y >= 0) && ((acos(y / r) + M_PI / 2.0) > Element->ParameterC * (ACOSM1O180) / 2.0))
			{
				*Intercept = 0;
				PosRayOut[0] = 0.0;
				PosRayOut[1] = 0.0;
				PosRayOut[2] = 0.0;
				CosRayOut[0] = 0.0;
				CosRayOut[1] = 0.0;
				CosRayOut[2] = 0.0;
				DFXYZ[0] = 0.0;
				DFXYZ[1] = 0.0;
				DFXYZ[2] = 0.0;
				*PathLength = 0.0;
				*BacksideFlag = 0;
				*ErrorFlag = 0;
				// goto Label_100;
				break;
			}
			else if ((y < 0) && ((-acos(-y / r) - M_PI / 2.0) < -Element->ParameterC * (ACOSM1O180) / 2.0))
			{
				*Intercept = 0;
				PosRayOut[0] = 0.0;
				PosRayOut[1] = 0.0;
				PosRayOut[2] = 0.0;
				CosRayOut[0] = 0.0;
				CosRayOut[1] = 0.0;
				CosRayOut[2] = 0.0;
				DFXYZ[0] = 0.0;
				DFXYZ[1] = 0.0;
				DFXYZ[2] = 0.0;
				*PathLength = 0.0;
				*BacksideFlag = 0;
				*ErrorFlag = 0;
				// goto Label_100;
				break;
			}

			if (DOT(CosRayIn, DFXYZ) < 0) // successfully falls on annular section
				*BacksideFlag = 0;
			else
				*BacksideFlag = 1;
			*Intercept = 1;
			// goto Label_100;
			break;
		}
		break;

	case 'l':
	case 'L': // off axis aperture section of line focus trough  or cylinder
		if ((Element->ParameterA == 0.0) && (Element->ParameterB == 0.0))
		{
			// Skipping goto (now goes to label 10 automatically)
			// goto Label_10; //for cylinder, only need to check for limits on y
		}
		else if ((x < Element->ParameterA) || (x > Element->ParameterB))
		{
			*Intercept = 0;
			PosRayOut[0] = 0.0;
			PosRayOut[1] = 0.0;
			PosRayOut[2] = 0.0;
			CosRayOut[0] = 0.0;
			CosRayOut[1] = 0.0;
			CosRayOut[2] = 0.0;
			DFXYZ[0] = 0.0;
			DFXYZ[1] = 0.0;
			DFXYZ[2] = 0.0;
			*PathLength = 0.0;
			*BacksideFlag = 0;
			*ErrorFlag = 0;
			// goto Label_100;
			break;
		}

		// Label_10:
		if ((y < -Element->ParameterC / 2.0) || (y > Element->ParameterC / 2.0))
		{
			*Intercept = 0;
			PosRayOut[0] = 0.0;
			PosRayOut[1] = 0.0;
			PosRayOut[2] = 0.0;
			CosRayOut[0] = 0.0;
			CosRayOut[1] = 0.0;
			CosRayOut[2] = 0.0;
			DFXYZ[0] = 0.0;
			DFXYZ[1] = 0.0;
			DFXYZ[2] = 0.0;
			*PathLength = 0.0;
			*BacksideFlag = 0;
			*ErrorFlag = 0;
			// goto Label_100;
			break;
		}

		if (DOT(CosRayIn, DFXYZ) < 0) // successfully falls on line focus or cylindrical section
			*BacksideFlag = 0;
		else
			*BacksideFlag = 1;

		*Intercept = 1;
		// goto Label_100;
		break;

	case 'i':
	case 'I': // irregular triangle
		P1x = Element->ParameterA;
		P1y = Element->ParameterB;
		P2x = Element->ParameterC;
		P2y = Element->ParameterD;
		P3x = Element->ParameterE;
		P3y = Element->ParameterF;

		if (!intri(P1x, P1y, P2x, P2y, P3x, P3y, x, y))
		{
			*Intercept = 0;
			PosRayOut[0] = 0.0;
			PosRayOut[1] = 0.0;
			PosRayOut[2] = 0.0;
			CosRayOut[0] = 0.0;
			CosRayOut[1] = 0.0;
			CosRayOut[2] = 0.0;
			DFXYZ[0] = 0.0;
			DFXYZ[1] = 0.0;
			DFXYZ[2] = 0.0;
			*PathLength = 0.0;
			*ErrorFlag = 0;
			*BacksideFlag = 0;
			// goto Label_100;
			break;
		}

		if (DOT(CosRayIn, DFXYZ) < 0)
			*BacksideFlag = 0;
		else
			*BacksideFlag = 1;
		*Intercept = 1;
		// goto Label_100;
		break;

	case 'q':
	case 'Q': // irregular quadrilateral
		P1x = Element->ParameterA;
		P1y = Element->ParameterB;
		P2x = Element->ParameterC;
		P2y = Element->ParameterD;
		P3x = Element->ParameterE;
		P3y = Element->ParameterF;
		P4x = Element->ParameterG;
		P4y = Element->ParameterH;

		in_quad = inquad(P1x, P1y, P2x, P2y, P3x, P3y, P4x, P4y, x, y);

		if (!in_quad)
		{
			*Intercept = 0;
			PosRayOut[0] = 0.0;
			PosRayOut[1] = 0.0;
			PosRayOut[2] = 0.0;
			CosRayOut[0] = 0.0;
			CosRayOut[1] = 0.0;
			CosRayOut[2] = 0.0;
			DFXYZ[0] = 0.0;
			DFXYZ[1] = 0.0;
			DFXYZ[2] = 0.0;
			*PathLength = 0.0;
			*ErrorFlag = 0;
			*BacksideFlag = 0;
			// goto Label_100;
			break;
		}

		if (DOT(CosRayIn, DFXYZ) < 0)
			*BacksideFlag = 0;
		else
			*BacksideFlag = 1;
		*Intercept = 1;
		// goto Label_100;
		break;
	} // end select case

	// Label_100:
	if (*BacksideFlag) // if hit on backside of element then slope of surface is reversed
	{
		DFXYZ[0] = -DFXYZ[0];
		DFXYZ[1] = -DFXYZ[1];
		DFXYZ[2] = -DFXYZ[2];
	}
}

int intri(double x1, double y1,
		  double x2, double y2,
		  double x3, double y3,
		  double xt, double yt)
{
	double a = (x1 - xt) * (y2 - yt) - (x2 - xt) * (y1 - yt);
	double b = (x2 - xt) * (y3 - yt) - (x3 - xt) * (y2 - yt);
	double c = (x3 - xt) * (y1 - yt) - (x1 - xt) * (y3 - yt);
	return (sign(a) == sign(b) && sign(b) == sign(c));
}

int inquad(double x1, double y1,
		   double x2, double y2,
		   double x3, double y3,
		   double x4, double y4,
		   double xt, double yt)
{
	return intri(x1, y1, x2, y2, x3, y3, xt, yt) || intri(x1, y1, x3, y3, x4, y4, xt, yt);
}

void Intersect(double PosLoc[3],
			   double CosLoc[3],
			   TElement *Element,
			   double PosXYZ[3],
			   double CosKLM[3],
			   double DFXYZ[3],
			   double *PathLength,
			   int *ErrorFlag)
{
	/*{Purpose: To compute intersection point and direction numbers for surface normal
	at intersection point of ray and surface. Path length is also computed.  From Spencer & Murty paper pg. 674
	   Input - PosLoc[3] = Initial position of ray in local coordinate system.
			   CosLoc[3] = Initial direction cosines of ray in local system.
			   Element.SurfaceType = Surface type flag
							 = 1 for rotationally symmetric surfaces
							 = 2 for torics and cylinders
							 = 3 for plane surfaces
							 = 4 for finite element data surface
							 = 5 for VSHOT data surface
							 = 6 for Zernike Monomial description
							 = 7 for single axis curvature surfaces
							 = 8 for rotationally symmetric polynomial description
							 = 9 for      "          "         cubic spline interpolation
							 =10 for torus
			   Element.Alpha = Sensitivity coefficients which specify deviation from conic
					   of revolution. For plane p = kx+ly+mz, Alpha[1] = p, Alpha{2..4] = k,l,m
			   Element.VertexCurvX = Vertex Curvature of surface
			   Element.Kappa = Surface specifier
					 < 0         ==> Hyperboloid
					 = 0         ==> Paraboloid
					 > 0 and < 1 ==> Hemelipsoid of revolution about major axis
					 = 1         ==> Hemisphere
					 > 1         ==> Hemielipsoid of revolution about minor axis
			   Element.ConeHalfAngle = Half-angle of cone for cones or revolution or axicons
			   Element.CurvOfRev = Curvature of revolution

	   Output - PosXYZ[3] = X, Y, Z coordinate of ray/surface intersection
				CosKLM[3] = direction cosines of ray
				DFXYZ[3]  = direction numbers for the surface normal at the
							intersection point (partial derivatives with respect to
							X, Y, Z of surface equation).
				PathLength = Path length
				ErrorFlag  = Error flag
							 = 0 for no errors
							 = 1 for Newton-Raphson iteration failed to converge
							 = 2 for interpolation error in SURFACE procedure} */
	int i = 0;
	double S0 = 0.0, S00 = 0.0, S0A = 0.0;
	double X1 = 0.0, x = 0.0, y = 0.0, r = 0.0;
	double Y10 = 0.0, Y1A = 0.0, X10 = 0.0, X1A = 0.0;
	double Y1 = 0.0;
	double SJ = 0.0;
	double SJ1 = 0.0;
	double DFDXYZ = 0.0;
	double FXYZ = 0.0;
	double ZStart = 0.0, ZA = 0.0;
	double ZStartcs = 0.0, PLengthcs = 0.0;
	int EFlagcs = 0;
	double OuterRadius = 0.0, InnerRadius = 0.0, R1 = 0.0, R1A = 0.0, R10 = 0.0, Z1 = 0.0, dzdR1 = 0.0;
	double S0Aperture = 0.0;
	double Ro = 0.0, Ri = 0.0, XL = 0.0;
	bool ZAInterceptInsideAperture = false;
	double Y2 = 0.0, Y3 = 0.0, Y4 = 0.0;
	double SLOP60 = 0.0, FXY = 0.0;
	double PosDum[3] = {0.0, 0.0, 0.0};
	double PosAtZA[3] = {0.0, 0.0, 0.0};
	double PosAtZ0[3] = {0.0, 0.0, 0.0};
	double P1x = 0.0, P1y = 0.0, P2x = 0.0, P2y = 0.0;
	double P3x = 0.0, P3y = 0.0, P4x = 0.0, P4y = 0.0;
	char ApertureShapeIndex = ' ';
	double PosInputToCS = 0.0;
	int in_quad = 0;

	*ErrorFlag = 0;
	for (i = 0; i < 3; i++)
	{
		PosXYZ[i] = PosLoc[i];
		CosKLM[i] = CosLoc[i];
	}

	// Closed form solutions used for closed surfaces (could use Newton-Raphson also,but would have to
	// pick the correct starting point (i.e. the initial point itself) to converge on first intersection
	// chose closed for cylinder
	if (Element->SurfaceType == 2) // cylinder
	{
		QuadricSurfaceClosedForm(Element, PosLoc, CosLoc, PosXYZ, DFXYZ, PathLength, ErrorFlag);
		return;
	}

	// wendelin 5-26-11 chose not use closed form solution for sphere.
	// this solves for a full spheroid, but can build a full spheroid from two hemispheres with iterative solution
	// JM 6/2023: Using closed form solution for sphere with single axis curvature aperture to avoid numerical problems caused by a bad starting point for Newton-Raphson (algorithm needs to start at a location with a defined z-location on the surface)
	if ((Element->SurfaceType == 1 || Element->SurfaceType == 7) && (Element->SurfaceIndex == 's' || Element->SurfaceIndex == 'S')) // sphere or partial cylinder
	{
		QuadricSurfaceClosedForm(Element, PosLoc, CosLoc, PosXYZ, DFXYZ, PathLength, ErrorFlag);
		return;
	}

	// JM 6/2023: Adding closed form solution for hyperboloids and hemi-ellipsoids
	if ((Element->SurfaceType == 1) && (Element->SurfaceIndex == 'o' || Element->SurfaceIndex == 'O')) // hyperboloid or hemiellipsoid
	{
		QuadricSurfaceClosedForm(Element, PosLoc, CosLoc, PosXYZ, DFXYZ, PathLength, ErrorFlag);
		return;
	}

	// JM 6/2023: Adding closed form solution for parabolas
	if ((Element->SurfaceIndex == 'p' || Element->SurfaceIndex == 'P'))
	{
		QuadricSurfaceClosedForm(Element, PosLoc, CosLoc, PosXYZ, DFXYZ, PathLength, ErrorFlag);
		return;
	}

	if (Element->SurfaceType == 10) // torus
	{
		TorusClosedForm(Element, PosLoc, CosLoc, PosXYZ, DFXYZ, PathLength, ErrorFlag);
		return;
	}

	//--------end of closed form solutions-------------
	//  {If not doing closed form solution, proceed to iterative solution}

	// start of new block for determining starting plane for Newton-Raphson   03-11-03

	/*{First, find starting plane.  The correct choice depends on the z-direction of the ray and the original
	position of the ray relative to the element surface.  First step is to find the intersection point
	of ray with  the element aperture plane and determine if it is inside or outside the aperture.
	Next, find z value of surface at x,y coords of original position.
	This determines which side of the surface equation the original position is. Then proceed through conditionals
	to determine the correct starting plane for Newton-Raphson.} */

	if (Element->ZAperture - PosXYZ[2] == 0.0) // numerical fix? 11-16-06 Tim Wendelin
		S0Aperture = 0.0;
	else
		S0Aperture = (Element->ZAperture - PosXYZ[2]) / (CosKLM[2] + 0.00000000001); // numerical fix? tim wendelin 11-20-06

	x = PosXYZ[0] + CosKLM[0] * S0Aperture; // x,y and radial position in aperture plane
	y = PosXYZ[1] + CosKLM[1] * S0Aperture;
	r = sqrt(x * x + y * y);

	// Determine if intersection point of ray with aperture plane falls inside element aperture
	SLOP60 = 1.7320508075688767; // tan(60.0*(ACOSM1O180));

	ZAInterceptInsideAperture = false;

	switch (Element->ShapeIndex)
	{
	case 'c':
	case 'C': // Circular aperture
		Ro = Element->ParameterA / 2.0;
		if (r > Ro) // ray falls outsideside circular aperture
			ZAInterceptInsideAperture = false;
		else
			ZAInterceptInsideAperture = true;
		break;

	case 'h':
	case 'H': // hexagonal aperture
		Ro = Element->ParameterA / 2.0;

		if (r > Ro) // ray falls outside circular circumference aperture
		{
			ZAInterceptInsideAperture = false;
			// goto Label_5;
			break;
		}

		Ri = Ro * cos(30.0 * (ACOSM1O180));

		if (r <= Ri) // ray falls inside inscribed circle
		{
			ZAInterceptInsideAperture = true;
			// goto Label_5;
			break;
		}

		XL = sqrt(Ro * Ro - Ri * Ri); // otherwise break hexagon into 3 sections
		if (x <= Ro && x > XL)		  // 1st section
		{
			Y1 = SLOP60 * (x - Ro);
			Y2 = -Y1;
			if (y >= Y1 && y <= Y2)
			{
				ZAInterceptInsideAperture = true;
				// goto Label_5;
				break;
			}

			ZAInterceptInsideAperture = false;
			// goto Label_5;
			break;
		}

		if (x <= XL && x >= -XL) // 2nd section
		{
			if (y >= -Ri && y <= Ri)
			{
				ZAInterceptInsideAperture = true;
				// goto Label_5;
				break;
			}

			ZAInterceptInsideAperture = false;
			// goto Label_5;
			break;
		}

		if (x < -XL && x >= -Ro) // 3rd section
		{
			Y3 = SLOP60 * (x + Ro);
			Y4 = -Y3;
			if (y >= Y4 && y <= Y3)
			{
				ZAInterceptInsideAperture = true;
				// goto Label_5;
				break;
			}

			ZAInterceptInsideAperture = false;
			// goto Label_5;
			break;
		}
		break;

	case 't':
	case 'T': // Triangular aperture
		Ro = Element->ParameterA / 2.0;

		if (r > Ro) // ray falls outside circular circumference aperture
		{
			ZAInterceptInsideAperture = false;
			// goto Label_5;
			break;
		}

		Ri = Ro * sin(30.0 * (ACOSM1O180));

		if (r <= Ri) // ray falls inside inscribed circle
		{
			ZAInterceptInsideAperture = true;
			// goto Label_5;
			break;
		}

		if (x <= Ro && x > 0.0) // 1st section
		{
			Y1 = -SLOP60 * (x - Ri / cos(30.0 * (ACOSM1O180)));
			Y2 = -Ri;
			if (y <= Y1 && y >= Y2)
				ZAInterceptInsideAperture = true;
			else
				ZAInterceptInsideAperture = false;
			// goto Label_5;
			break;
		}

		if (x >= -Ro && x <= 0.0) // 2nd section
		{
			Y3 = SLOP60 * (x + Ri / cos(30.0 * (ACOSM1O180)));
			Y4 = -Ri;
			if (y >= Y4 && y <= Y3)
				ZAInterceptInsideAperture = true;
			else
				ZAInterceptInsideAperture = false;

			// goto Label_5;
			break;
		}
		break;

	case 'r':
	case 'R': // Rectangular aperture

		if (x > Element->ParameterA / 2.0 && x < -Element->ParameterA / 2.0)
		{
			ZAInterceptInsideAperture = false;
			// goto Label_5;
			break;
		}

		if (y > Element->ParameterB / 2.0 && y < -Element->ParameterB / 2.0)
		{
			ZAInterceptInsideAperture = false;
			// goto Label_5;
			break;
		}

		ZAInterceptInsideAperture = true;
		// goto Label_5;

		break;

	case 'a':
	case 'A': // Annulus

		if (r < Element->ParameterA || r > Element->ParameterB)
		{
			ZAInterceptInsideAperture = false;
			// goto Label_5;
			break;
		}

		if (x >= 0.0)
		{
			if ((asin(y / r) > Element->ParameterC * (ACOSM1O180) / 2.0) || (asin(y / r) < -Element->ParameterC * (ACOSM1O180) / 2.0))
				ZAInterceptInsideAperture = false;
			else
				ZAInterceptInsideAperture = true;
			// goto Label_5;
			break;
		}

		if (x < 0.0)
		{
			if ((y >= 0) && ((acos(y / r) + M_PI / 2.0) > Element->ParameterC * (ACOSM1O180) / 2.0))
			{
				ZAInterceptInsideAperture = false;
				// goto Label_5;
				break;
			}
			else if ((y < 0) && ((-acos(-y / r) - M_PI / 2.0) < -Element->ParameterC * (ACOSM1O180) / 2.0))
			{
				ZAInterceptInsideAperture = false;
				// goto Label_5;
				break;
			}

			ZAInterceptInsideAperture = true;
			// goto Label_5;
			break;
		}

		break;

	case 'l':
	case 'L': // off axis aperture section of line focus trough  or cylinder

		if (Element->ParameterA == 0.0 && Element->ParameterB == 0.0)
		{
			// goto Label_4; //for cylinder, only need to check for limits on y
		}
		else if (x < Element->ParameterA || x > Element->ParameterB)
		{
			ZAInterceptInsideAperture = false;
			// goto Label_5;
			break;
		}

		// Label_4:
		if (y < -Element->ParameterC / 2.0 || y > Element->ParameterC / 2.0)
		{
			ZAInterceptInsideAperture = false;
			// goto Label_5;
			break;
		}

		ZAInterceptInsideAperture = true;
		// goto Label_5;

		break;

	case 'i':
	case 'I': // irregular triangle
		P1x = Element->ParameterA;
		P1y = Element->ParameterB;
		P2x = Element->ParameterC;
		P2y = Element->ParameterD;
		P3x = Element->ParameterE;
		P3y = Element->ParameterF;

		if (!intri(P1x, P1y, P2x, P2y, P3x, P3y, x, y))
		{
			ZAInterceptInsideAperture = false;
			// goto Label_5;
			break;
		}

		ZAInterceptInsideAperture = true;
		// goto Label_5;
		break;

	case 'q':
	case 'Q': // irregular quadrilateral

		P1x = Element->ParameterA;
		P1y = Element->ParameterB;
		P2x = Element->ParameterC;
		P2y = Element->ParameterD;
		P3x = Element->ParameterE;
		P3y = Element->ParameterF;
		P4x = Element->ParameterG;
		P4y = Element->ParameterH;

		in_quad = inquad(P1x, P1y, P2x, P2y, P3x, P3y, P4x, P4y, x, y);

		if (!in_quad)
		{
			ZAInterceptInsideAperture = false;
			// goto Label_5;
			break;
		}

		ZAInterceptInsideAperture = true;
		// goto Label_5;
		break;
	default:
		break;
	} // end switch

	// Label_5:
	//	if (in_quad && !ZAInterceptInsideAperture)
	//		printf("ERROR\n");

	ZStart = 0.0; // default for all surfacetypes

	if (Element->SurfaceType != 3 && Element->SurfaceType != 4 && Element->SurfaceType != 9)
	{
		SurfaceZatXYPair(PosXYZ, Element, &FXY, ErrorFlag); // find z value of surface at x,y

		if (PosXYZ[2] <= 0.0 && CosKLM[2] > 0.0) // if ray position below z=0 and pointing up then
		{										 // ZStart should be z=0 plane.
			ZStart = 0.0;
			// goto Label_10;
		}

		else if (PosXYZ[2] <= FXY && CosKLM[2] > 0.0) // if ray position is below surface equation and pointing up
		{											  // then ZStart should be z=0 plane.
			ZStart = 0.0;
			// goto Label_10;
		}

		else if (PosXYZ[2] <= FXY && CosKLM[2] < 0.0 && PosXYZ[2] > Element->ZAperture && ZAInterceptInsideAperture)
		{				  // if ray position is below surface equation, above the aperture
			ZStart = 0.0; // plane and pointing down
						  // goto Label_10;                                            //and the interception point with aperture plane is inside of
		} // aperture, then ZStart should be z=0 plane.

		else if (PosXYZ[2] <= FXY && CosKLM[2] < 0.0) // if ray position is below surface equation, pointing down
		{											  // and hits surface below aperture plane then ZStart should be
			ZStart = Element->ZAperture;			  // aperture plane.
													  // goto Label_10;
		}

		else if (PosXYZ[2] > FXY && CosKLM[2] < 0.0) // if ray position is above surface and pointing in negative z
		{											 // direction then ZStart should be z=0 plane
			ZStart = 0.0;
			// goto Label_10;
		}

		else if (PosXYZ[2] > FXY && CosKLM[2] > 0.0)
			ZStart = Element->ZAperture; // if ray position is above the surface and
	} // pointing up then ZStart should be

	// // The following calculates ZStart for surfaces described by cubic spline data only.
	// if (Element->SurfaceType == 9)
	// {
	// 	OuterRadius = Element->CubicSplineXData[Element->CubicSplineXData.size() - 1]; // outer,inner radii (or distance from origin if single axis curvature) of data set
	// 	InnerRadius = Element->CubicSplineXData[0];
	// 	ApertureShapeIndex = Element->ShapeIndex;
	// 	ZA = Element->CubicSplineYData[Element->CubicSplineYData.size() - 1]; // z value at aperture plane ZA

	// 	S00 = -PosXYZ[2] / (CosKLM[2] + 0.00000000001); // numerical fix? tim wendelin 11-20-06; //pathlength from original ray point to z=0 plane

	// 	X10 = PosXYZ[0] + CosKLM[0] * S00; // x,y location of intersection point in z=0 plane
	// 	Y10 = PosXYZ[1] + CosKLM[1] * S00;
	// 	R10 = sqrt(X10 * X10 + Y10 * Y10); // radius of intersection point in z=0 plane

	// 	S0A = (ZA - PosXYZ[2]) / (CosKLM[2] + 0.00000000001); // numerical fix? tim wendelin 11-20-06;  //pathlength from original ray point to aperture plane

	// 	X1A = PosXYZ[0] + CosKLM[0] * S0A; // x,y location of intersection point in aperture plane
	// 	Y1A = PosXYZ[1] + CosKLM[1] * S0A;
	// 	R1A = sqrt(X1A * X1A + Y1A * Y1A); // radius of intersection point in aperture plane

	// 	// original location and direction of ray defines starting plane for Newton-Raphson.  This is split into several
	// 	// sections as can be seen in the following.

	// 	// ray at or above aperture plane, ZA, and heading toward Z0
	// 	if (PosXYZ[2] >= ZA && CosKLM[2] < 0.0)
	// 	{
	// 		// move starting point for ray to aperture plane, so intersects at correct point on cylinder below,  03-20-04
	// 		PosAtZA[0] = X1A;
	// 		PosAtZA[1] = Y1A;
	// 		PosAtZA[2] = ZA;

	// 		//{check R or X position depending if rotationally symmetric curvature or single axis curvature}
	// 		if ((((ApertureShapeIndex == 'a') || (ApertureShapeIndex == 'A')) && (R1A > OuterRadius)) || (((ApertureShapeIndex == 'l') || (ApertureShapeIndex == 'L')) && (X1A > OuterRadius)))
	// 		{
	// 			// find intersection with cylinder at outside edge of dataset.  The z value becomes the new ZStart.
	// 			// NewZStartforCubicSplineSurf(OuterRadius*0.999999, PosLoc, CosLoc, ZStartcs, PLengthcs, EFlagcs); //see PosAtZA comment above
	// 			NewZStartforCubicSplineSurf(OuterRadius * 0.999999, PosAtZA, CosLoc, ApertureShapeIndex, &ZStartcs, &PLengthcs, &EFlagcs);
	// 			if (EFlagcs == 0)
	// 			{
	// 				ZStart = ZStartcs;
	// 				// goto Label_10;
	// 			}
	// 			// ray misses virtual cylinder so move on.
	// 			// goto Label_10;
	// 		}

	// 		//{check R or X position depending if rotationally symmetric curvature or single axis curvature}
	// 		else if ((((ApertureShapeIndex == 'a') || (ApertureShapeIndex == 'A')) && (R1A <= OuterRadius)) || (((ApertureShapeIndex == 'l') || (ApertureShapeIndex == 'L')) && (X1A <= OuterRadius)))
	// 		{
	// 			if ((((ApertureShapeIndex == 'a') || (ApertureShapeIndex == 'A')) && (R10 >= OuterRadius)) || (((ApertureShapeIndex == 'l') || (ApertureShapeIndex == 'L')) && (X10 >= OuterRadius)))
	// 			{
	// 				// find intersection with cylinder at outside edge of dataset.  The z value becomes the new ZStart.
	// 				// NewZStartforCubicSplineSurf(OuterRadius*0.999999, PosLoc, CosLoc, ZStartcs, PLengthcs, EFlagcs); //see PosAtZA comment above
	// 				NewZStartforCubicSplineSurf(OuterRadius * 0.999999, PosAtZA, CosLoc, ApertureShapeIndex, &ZStartcs, &PLengthcs, &EFlagcs);
	// 				if (EFlagcs == 0)
	// 				{
	// 					ZStart = ZStartcs;
	// 					// goto Label_10;
	// 				}

	// 				// ray misses virtual cylinder so move on.
	// 				// goto Label_10;
	// 			}

	// 			ZStart = 0.0;
	// 			// goto Label_10;
	// 		}
	// 	}

	// 	// ray at or below Z0 plane and heading toward ZA
	// 	else if (PosXYZ[2] <= 0.0 && CosKLM[2] > 0.0)
	// 	{
	// 		// move starting point for ray to z=0 plane, so intersects at correct point on cylinder below     03/20/04
	// 		PosAtZ0[0] = X10;
	// 		PosAtZ0[1] = Y10;
	// 		PosAtZ0[2] = 0.0;
	// 		// {check R or X position depending if rotationally symmetric curvature or single axis curvature}
	// 		if ((((ApertureShapeIndex == 'a') || (ApertureShapeIndex == 'A')) && (R10 >= OuterRadius)) || (((ApertureShapeIndex == 'l') || (ApertureShapeIndex == 'L')) && (X10 >= OuterRadius)))
	// 		{
	// 			// find intersection with cylinder at outside edge of dataset.  The z value becomes the new ZStart.
	// 			// NewZStartforCubicSplineSurf(OuterRadius*0.999999, PosLoc, CosLoc, ZStartcs, PLengthcs, EFlagcs); //see PosAtZ0 comment above
	// 			NewZStartforCubicSplineSurf(OuterRadius * 0.999999, PosAtZ0, CosLoc, ApertureShapeIndex, &ZStartcs, &PLengthcs, &EFlagcs);
	// 			if (EFlagcs == 0)
	// 			{
	// 				ZStart = ZStartcs;
	// 				// goto Label_10;
	// 			}
	// 			// ray misses virtual cylinder so move on.
	// 			// goto Label_10;
	// 		}

	// 		//{check R or X position depending if rotationally symmetric curvature or single axis curvature}
	// 		else if ((((ApertureShapeIndex == 'a') || (ApertureShapeIndex == 'A')) && ((R10 < OuterRadius) && (R10 > InnerRadius))) || (((ApertureShapeIndex == 'l') || (ApertureShapeIndex == 'L')) && ((X10 < OuterRadius) && (X10 > InnerRadius))))
	// 		{
	// 			ZStart = 0.0;
	// 			// goto Label_10;
	// 		}

	// 		//{check R or X position depending if rotationally symmetric curvature or single axis curvature}
	// 		else if ((((ApertureShapeIndex == 'a') || (ApertureShapeIndex == 'A')) && (R10 <= InnerRadius)) || (((ApertureShapeIndex == 'l') || (ApertureShapeIndex == 'L')) && (X10 <= InnerRadius)))
	// 		{
	// 			// find intersection with cylinder at inside edge of dataset.  The z value becomes the new ZStart.
	// 			// NewZStartforCubicSplineSurf(InnerRadius/0.999999, PosLoc, CosLoc, ZStartcs, PLengthcs, EFlagcs); //see PosAtZ0 comment above
	// 			NewZStartforCubicSplineSurf(InnerRadius / 0.999999, PosAtZ0, CosLoc, ApertureShapeIndex, &ZStartcs, &PLengthcs, &EFlagcs);
	// 			if (EFlagcs == 0)
	// 			{
	// 				ZStart = ZStartcs;
	// 				// goto Label_10;
	// 			}
	// 			// ray misses virtual cylinder so move on.
	// 			// goto Label_10;
	// 		}
	// 	}

	// 	// ray in between ZA and Z0 planes and headed towared Z0
	// 	else if (PosXYZ[2] < ZA && PosXYZ[2] > 0.0 && CosKLM[2] < 0.0)
	// 	{
	// 		R1 = sqrt(PosXYZ[0] * PosXYZ[0] + PosXYZ[1] * PosXYZ[1]); // ray radial position
	// 		//{check R or X position depending if rotationally symmetric curvature or single axis curvature}
	// 		if ((((ApertureShapeIndex == 'a') || (ApertureShapeIndex == 'A')) && (R1 >= OuterRadius)) || (((ApertureShapeIndex == 'l') || (ApertureShapeIndex == 'L')) && (PosXYZ[0] >= OuterRadius))) // ray radial position outside of dataset
	// 		{
	// 			// find intersection with cylinder at outside edge of dataset.  The z value becomes the new ZStart.
	// 			NewZStartforCubicSplineSurf(OuterRadius * 0.999999, PosLoc, CosLoc, ApertureShapeIndex, &ZStartcs, &PLengthcs, &EFlagcs);
	// 			if (EFlagcs == 0)
	// 			{
	// 				ZStart = ZStartcs;
	// 				// goto Label_10;
	// 			}
	// 			// ray misses virtual cylinder so move on.
	// 			// goto Label_10;
	// 		}

	// 		//{check R or X position depending if rotationally symmetric curvature or single axis curvature}
	// 		else if ((((ApertureShapeIndex == 'a') || (ApertureShapeIndex == 'A')) && ((R1 < OuterRadius) && (R1 > InnerRadius))) || (((ApertureShapeIndex == 'l') || (ApertureShapeIndex == 'L')) && ((PosXYZ[0] < OuterRadius) && (PosXYZ[0] > InnerRadius)))) // ray radial position within dataset boundaries
	// 		{																																																													 // find z value at x,y. this determines if point is above or below curve
	// 			if (ApertureShapeIndex == 'a' || ApertureShapeIndex == 'A')
	// 				PosInputToCS = R1;
	// 			else
	// 				PosInputToCS = PosXYZ[0];

	// 			if (!splint(Element->CubicSplineXData,
	// 						Element->CubicSplineYData,
	// 						Element->CubicSplineY2Data,
	// 						Element->CubicSplineXData.size(),
	// 						PosInputToCS, &Z1, &dzdR1))
	// 			{
	// 				*ErrorFlag = 3;
	// 				return;
	// 			}

	// 			if (Z1 < PosXYZ[2]) // ray is above curve
	// 			{
	// 				//	 {check R or X position depending if rotationally symmetric curvature or single axis curvature}
	// 				if ((((ApertureShapeIndex == 'a') || (ApertureShapeIndex == 'A')) && (R10 >= OuterRadius)) || (((ApertureShapeIndex == 'l') || (ApertureShapeIndex == 'L')) && (X10 >= OuterRadius)))
	// 				{
	// 					// find intersection with cylinder at outside edge of dataset.  The z value becomes the new ZStart.
	// 					NewZStartforCubicSplineSurf(OuterRadius * 0.999999, PosLoc, CosLoc, ApertureShapeIndex, &ZStartcs, &PLengthcs, &EFlagcs);
	// 					if (EFlagcs == 0)
	// 					{
	// 						ZStart = ZStartcs;
	// 						// goto Label_10;
	// 					}
	// 					// ray misses virtual cylinder so move on.
	// 					// goto Label_10;
	// 				}
	// 				else
	// 				{
	// 					ZStart = 0.0;
	// 					// goto Label_10;
	// 				}
	// 			}

	// 			else if (Z1 >= PosXYZ[2]) // ray is below curve
	// 			{
	// 				ZStart = PosXYZ[2];
	// 				// ray misses virtual cylinder so move on.
	// 				// goto Label_10;
	// 			}
	// 			else
	// 			{
	// 				// goto Label_10;
	// 			}
	// 		}

	// 		//{check R or X position depending if rotationally symmetric curvature or single axis curvature}
	// 		else if ((((ApertureShapeIndex == 'a') || (ApertureShapeIndex == 'A')) && (R1 <= InnerRadius)) || (((ApertureShapeIndex == 'l') || (ApertureShapeIndex == 'L')) && (PosXYZ[0] <= InnerRadius))) // ray radial position inside of dataset
	// 		{
	// 			// find intersection with cylinder at inside edge of dataset.  The z value becomes the new ZStart.
	// 			NewZStartforCubicSplineSurf(InnerRadius / 0.999999, PosLoc, CosLoc, ApertureShapeIndex, &ZStartcs, &PLengthcs, &EFlagcs);
	// 			if (EFlagcs == 0)
	// 			{
	// 				ZStart = ZStartcs;
	// 				// goto Label_10;
	// 			}

	// 			// ray misses virtual cylinder so move on.
	// 			// goto Label_10;
	// 		}
	// 	}

	// 	// ray in between ZA and Z0 planes and headed toward ZA
	// 	else if (PosXYZ[2] < ZA && PosXYZ[2] > 0.0 && CosKLM[2] > 0.0)
	// 	{
	// 		R1 = sqrt(PosXYZ[0] * PosXYZ[0] + PosXYZ[1] * PosXYZ[1]); // ray radial position
	// 		// {check R or X position depending if rotationally symmetric curvature or single axis curvature}
	// 		if ((((ApertureShapeIndex == 'a') || (ApertureShapeIndex == 'A')) && (R1 >= OuterRadius)) || (((ApertureShapeIndex == 'l') || (ApertureShapeIndex == 'L')) && (PosXYZ[0] >= OuterRadius))) // ray radial position outside of dataset
	// 		{
	// 			// find intersection with cylinder at outside edge of dataset.  The z value becomes the new ZStart.
	// 			NewZStartforCubicSplineSurf(OuterRadius * 0.999999, PosLoc, CosLoc, ApertureShapeIndex, &ZStartcs, &PLengthcs, &EFlagcs);
	// 			if (EFlagcs == 0)
	// 			{
	// 				ZStart = ZStartcs;
	// 				// goto Label_10;
	// 			}
	// 			// ray misses virtual cylinder so move on.
	// 			// goto Label_10;
	// 		}

	// 		//  {check R or X position depending if rotationally symmetric curvature or single axis curvature}
	// 		else if ((((ApertureShapeIndex == 'a') || (ApertureShapeIndex == 'A')) && ((R1 < OuterRadius) && (R1 > InnerRadius))) || (((ApertureShapeIndex == 'l') || (ApertureShapeIndex == 'L')) && ((PosXYZ[0] < OuterRadius) && (PosXYZ[0] > InnerRadius)))) // ray radial position falls within dataset boundaries
	// 		{																																																													 // find z value at x,y. this determines if point is above or below curve

	// 			if (ApertureShapeIndex == 'a' || ApertureShapeIndex == 'A')
	// 				PosInputToCS = R1;
	// 			else
	// 				PosInputToCS = PosXYZ[0];

	// 			if (!splint(Element->CubicSplineXData,
	// 						Element->CubicSplineYData,
	// 						Element->CubicSplineY2Data,
	// 						Element->CubicSplineXData.size(),
	// 						PosInputToCS, &Z1, &dzdR1))
	// 			{
	// 				*ErrorFlag = 3;
	// 				return;
	// 			}

	// 			if (Z1 < PosXYZ[2]) // ray is above curve
	// 			{
	// 				//{check R or X position depending if rotationally symmetric curvature or single axis curvature}
	// 				if ((((ApertureShapeIndex == 'a') || (ApertureShapeIndex == 'A')) && (R1A >= OuterRadius)) || (((ApertureShapeIndex == 'l') || (ApertureShapeIndex == 'L')) && (X1A >= OuterRadius)))
	// 				{
	// 					// find intersection with cylinder at outside edge of dataset.  The z value becomes the new ZStart.
	// 					NewZStartforCubicSplineSurf(OuterRadius * 0.999999, PosLoc, CosLoc, ApertureShapeIndex, &ZStartcs, &PLengthcs, &EFlagcs);
	// 					if (EFlagcs == 0)
	// 					{
	// 						ZStart = ZStartcs;
	// 						// goto Label_10;
	// 					}
	// 					// ray misses virtual cylinder so move on.
	// 					// goto Label_10;
	// 				}
	// 				// goto Label_10;
	// 			}

	// 			else if (Z1 >= PosXYZ[2]) // ray is below curve
	// 			{
	// 				//{check R or X position depending if rotationally symmetric curvature or single axis curvature}
	// 				if ((((ApertureShapeIndex == 'a') || (ApertureShapeIndex == 'A')) && (R10 < OuterRadius)) || (((ApertureShapeIndex == 'l') || (ApertureShapeIndex == 'L')) && (X10 < OuterRadius)))
	// 				{
	// 					ZStart = 0.0;
	// 					// goto Label_10;
	// 				}

	// 				// {check R or X position depending if rotationally symmetric curvature or single axis curvature}
	// 				else if ((((ApertureShapeIndex == 'a') || (ApertureShapeIndex == 'A')) && (R10 >= OuterRadius)) || (((ApertureShapeIndex == 'l') || (ApertureShapeIndex == 'L')) && (X10 >= OuterRadius)))
	// 				{
	// 					PosDum[0] = X10; // back up to intersection with z=0 plane
	// 					PosDum[1] = Y10;
	// 					PosDum[2] = 0.0;
	// 					// find intersection with cylinder at outside edge of dataset.  The z value becomes the new ZStart.
	// 					NewZStartforCubicSplineSurf(OuterRadius * 0.999999, PosDum, CosLoc, ApertureShapeIndex, &ZStartcs, &PLengthcs, &EFlagcs);
	// 					if (EFlagcs == 0)
	// 					{
	// 						ZStart = ZStartcs;
	// 						// goto Label_10;
	// 					}
	// 					// ray misses virtual cylinder so move on.
	// 					// goto Label_10;
	// 				}
	// 			}
	// 		}

	// 		// {check R or X position depending if rotationally symmetric curvature or single axis curvature}
	// 		else if ((((ApertureShapeIndex == 'a') || (ApertureShapeIndex == 'A')) && (R1 <= InnerRadius)) || (((ApertureShapeIndex == 'l') || (ApertureShapeIndex == 'L')) && (PosXYZ[0] <= InnerRadius))) // ray radial position inside of dataset minimum radius
	// 		{
	// 			// find intersection with cylinder at outside edge of dataset.  The z value becomes the new ZStart.
	// 			NewZStartforCubicSplineSurf(OuterRadius * 0.999999, PosLoc, CosLoc, ApertureShapeIndex, &ZStartcs, &PLengthcs, &EFlagcs);
	// 			if (EFlagcs == 0)
	// 			{
	// 				ZStart = ZStartcs;
	// 				// goto Label_10;
	// 			}
	// 			// ray misses virtual cylinder so move on.
	// 			// goto Label_10;
	// 		}
	// 	}
	// }

	// Label_10:
	if (ZStart - PosXYZ[2] == 0.0) // numerical fix? 11-16-06 Tim Wendelin
		S0 = 0.0;
	else
		S0 = (ZStart - PosXYZ[2]) / (CosKLM[2] + 0.00000000001); // numerical fix? tim wendelin 11-20-06;   //SO is the pathlength from the initial ray position to the Newton-Raphson starting plane

	X1 = PosXYZ[0] + CosKLM[0] * S0; // from this we calculate the x,y position on ZStart starting plane
	Y1 = PosXYZ[1] + CosKLM[1] * S0;

	SJ1 = 0.0;

	// // JM 10/2023: Check upper and lower bounds for S (distance along ray path from (X1,Y1,Zstart)) to restrict step size for cubic spline
	// // This may slow down the solution - previously the iterations would find an intersection point for the spline, but it would be later disgarded because it is out of bounds, now the loop below will reach the iteration limit before failing is there is no intersection
	// double lower_bound = -1e10;
	// double upper_bound = 1e10;
	// if (Element->SurfaceType == 9)
	// {
	// 	double s_to_xmin = (Element->CubicSplineXData[0] - X1) / (CosKLM[0] + 0.00000000001);									 // Distance along ray path from (X1,Y1,Zstart) to smallest x-coordinate on surface
	// 	double s_to_xmax = (Element->CubicSplineXData[Element->CubicSplineXData.size() - 1] - X1) / (CosKLM[0] + 0.00000000001); // Distance along ray path from (X1,Y1,Zstart) to largest x-coordinate on surface
	// 	lower_bound = fmin(s_to_xmin, s_to_xmax);
	// 	upper_bound = fmax(s_to_xmin, s_to_xmax);
	// }

	i = 0;
	// Begin the Newton-Raphson Iteration
	int OKFlag = 0;

	const uint_fast64_t NumIterations = 20;
	const double Epsilon = 1e-6;

	while (i++ < NumIterations)
	{
		SJ = SJ1;
		PosXYZ[0] = X1 + CosKLM[0] * SJ;
		PosXYZ[1] = Y1 + CosKLM[1] * SJ;
		PosXYZ[2] = ZStart + CosKLM[2] * SJ;

		Surface(PosXYZ, Element, &FXYZ, DFXYZ, &OKFlag);

		// if (OKFlag == 0) goto Label_40;
		if (OKFlag != 0)
		{
			*ErrorFlag = 2; // Interpolation error in Surface procedure
			break;
			// goto Label_100;
		}

		// Label_40:
		DFDXYZ = DOT(DFXYZ, CosKLM);
		if (fabs(FXYZ) <= Epsilon * fabs(DFDXYZ))
		{
			break;
			// goto Label_100;
		}

		SJ1 = SJ - FXYZ / DFDXYZ;

		// // JM 10/2023: Enforce bounds to restrict next guess for cubic spline
		// if (Element->SurfaceType == 9)
		// {
		// 	if ((FXYZ < 0 && CosKLM[2] > 0) || (FXYZ > 0 && CosKLM[2] < 0)) // FXYZ < 0 if current point is below surface, FXYZ > 0 if current point is above surface
		// 		lower_bound = fmax(SJ, lower_bound);
		// 	else if ((FXYZ < 0 && CosKLM[2] < 0) || (FXYZ > 0 && CosKLM[2] > 0))
		// 		upper_bound = fmin(SJ, upper_bound);
		// 	if (SJ1 < lower_bound || SJ1 > upper_bound)
		// 		SJ1 = 0.5 * (lower_bound + upper_bound);
		// 	if (upper_bound <= lower_bound)
		// 	{
		// 		*ErrorFlag = 1; // Failed to converge
		// 		break;
		// 	}
		// }
	}
	if (i == NumIterations)
		*ErrorFlag = 1; // Failed to converge

	// Label_100:
	*PathLength = S0 + SJ;

	return;
}

void Errors(
	MTRand &myrng,
	double CosIn[3],
	int Source,
	TSun *Sun,
	TElement *Element,
	TOpticalProperties *OptProperties,
	double CosOut[3],
	double DFXYZ[3])
{
	/*{Purpose:  To add error terms to the perturbed ray at the surface in question

			   Input - Seed    = Seed for RNG
					   CosIn   = Direction cosine vector of ray to which errors will be applied.
								  If Source below is 1 (i.e. sunshape) then this ray vector is before interaction with element surface
								  If Source below is 2 (i.e. surface error) then this ray vector is after interaction with element surface
									(i.e. reflected ray or transmitted ray)

					   Source  = Source indicator flag
							   = 1 for Sunshape error (Can be gaussian, pillbox or profile data distribution)
							   = 2 for surface errors (Can be gaussian or pillbox distribution)
					   Sun     = Sun data record
					   Element = Element data record
					   DFXYZ   = surface normal vector at interaction point

			   Output - CosOut  = Output direction cosine vector of ray after error terms have been included
					   }*/
	double Origin[3] = {0.0, 0.0, 0.0};
	double Euler[3] = {0.0, 0.0, 0.0};
	double PosIn[3] = {0.0, 0.0, 0.0};
	double PosOut[3] = {0.0, 0.0, 0.0};
	char dist = 'g';
	double delop = 0, delop3 = 0, thetax = 0, thetay = 0, ttheta = 0, theta2 = 0, phi = 0, theta = 0, stest = 0;
	uint_fast64_t i;
	double RRefToLoc[3][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
	double RLocToRef[3][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};

	if (CosIn[2] == 0.0)
	{
		if (CosIn[0] == 0.0)
		{
			Euler[0] = 0.0;
			Euler[1] = M_PI / 2.0;
			goto Label_9;
		}
		else
		{
			Euler[0] = M_PI / 2.0;
			goto Label_8;
		}
	}

	Euler[0] = atan2(CosIn[0], CosIn[2]);

Label_8:
	Euler[1] = atan2(CosIn[1], sqrt(CosIn[0] * CosIn[0] + CosIn[2] * CosIn[2]));

Label_9:
	Euler[2] = 0.0;

	CalculateTransformMatrices(Euler, RRefToLoc, RLocToRef);

	// g,p,d
	if (Source == 1)
	{
		dist = Sun->ShapeIndex; // sun
		delop = Sun->Sigma / 1000.0;
	}

	if (Source == 2)
	{
		dist = OptProperties->DistributionType; // errors
		// delop = sqrt(4.0*sqr(OptProperties->RMSSlopeError)+sqr(OptProperties->RMSSpecError))/1000.0;
		delop = OptProperties->RMSSpecError / 1000.0;
	}

	unsigned int maxcall = 0;

Label_50:
	switch (dist)
	{
	case 'g':
	case 'G': // gaussian distribution
		thetax = myrng.randNorm(0., delop);
		thetay = myrng.randNorm(0., delop);

		theta2 = thetax * thetax + thetay * thetay;

		break;

	case 'p':
	case 'P': // pillbox distribution
	Label_200:
		thetax = 2.0 * delop * myrng() - delop;
		thetay = 2.0 * delop * myrng() - delop;
		theta2 = thetax * thetax + thetay * thetay;
		if (theta2 > (delop * delop))
			goto Label_200;
		break;

	case 'd':
	case 'D': // sunshape data  (for sunshape only)
	Label_300:
		thetax = 2.0 * Sun->MaxAngle * myrng() - Sun->MaxAngle;
		thetay = 2.0 * Sun->MaxAngle * myrng() - Sun->MaxAngle;
		theta2 = thetax * thetax + thetay * thetay;
		theta = sqrt(theta2); // wendelin 1-9-12  do the test once on theta NOT individually on thetax and thetay as before

		i = 0;
		while (i < Sun->SunShapeAngle.size() - 1 && Sun->SunShapeAngle[i] < theta)
			i++;

		if (i == 0)
			stest = Sun->SunShapeIntensity[0];
		else // change from average interpolation between data points to linear interpolation  12-20-11 wendelin
			stest = Sun->SunShapeIntensity[i - 1] + (Sun->SunShapeIntensity[i] - Sun->SunShapeIntensity[i - 1]) * (theta - Sun->SunShapeAngle[i - 1]) /
														(Sun->SunShapeAngle[i] - Sun->SunShapeAngle[i - 1]);
		// stest = (Sun->SunShapeIntensity[i] + Sun->SunShapeIntensity[i-1])/2.0;

		if (myrng() > (stest / Sun->MaxIntensity))
			goto Label_300;

		if (theta2 > (Sun->MaxAngle * Sun->MaxAngle))
			goto Label_300;
		theta2 = theta2 / 1000000.0;
		break;

	case 'f': // gray diffuse distribution
	case 'F':
		theta2 = pow(asin(sqrt(myrng())), 2);
		break;
	}

	/*{Transform to local coordinate system of ray to set up rotation matrices for coord and inverse
	  transforms}*/
	TransformToLocal(PosIn, CosIn, Origin, RRefToLoc, PosOut, CosOut);

	// {Generate errors in terms of direction cosines in local ray coordinate system}
	theta = sqrt(theta2);

	// phi = atan2(thetay, thetax); //This function appears to  present irregularities that bias results incorrectly for small values of thetay or thetax
	phi = myrng() * 2.0 * 3.1415926535897932385; // Therefore have chosen to randomize phi rather than calculate from randomized theta components
												 //  obtained from the distribution. The two approaches are equivalent save for this issue with
												 //  arctan2.      wendelin 01-12-11

	CosOut[0] = sin(theta) * cos(phi);
	CosOut[1] = sin(theta) * sin(phi);
	CosOut[2] = cos(theta);

	for (i = 0; i < 3; i++)
	{
		PosIn[i] = PosOut[i];
		CosIn[i] = CosOut[i];
	}

	//{Transform perturbed ray back to element system}
	TransformToReference(PosIn, CosIn, Origin, RLocToRef, PosOut, CosOut);

	/*{If reflection error applicaton and new ray direction (after errors) physically goes through opaque surface,
	then go back and get new perturbation 06-12-07}*/
	if ((Source == 2) && (Element->InteractionType == 2) && (DOT(CosOut, DFXYZ) < 0) && maxcall++ < 50000)
		goto Label_50;
}
// End of Procedure--------------------------------------------------------------

void Surface(
	double PosXYZ[3],
	TElement *Element,
	double *FXYZ,
	double DFXYZ[3],
	int *ErrorFlag)
{
	/*{Purpose: To compute the surface equation and it's derivatives for various
	geometric surfaces.
		Input - PosXYZ[3] = X, Y, Z coordinate position
				Element.SurfaceType = Surface type flag
							  = 1 for rotationally symmetric surfaces
							  = 2 for torics and cylinders
							  = 3 for plane surfaces
							  = 4 for surface interpolated from finite element data points
							  = 5 for surface interpolated from VSHOT data points
							  = 6 for surface described by Zernike monomials
							  = 7 for single axis parabolic curvature surfaces
							  = 8 for rotationally symmetric polynomial description
							  = 9 for       "          "     cubic spline interpolation
				Element.Alpha = Sensitivity coefficients which specify deviation from conic
						of revolution
				Element.VertexCurvX = Vertex Curvature of surface
				Element.Kappa = Surface specifier
					 < 0         ==> Hyperboloid
					 = 0         ==> Paraboloid
					 > 0 and < 1 ==> Hemelipsoid of revolution about major axis
					 = 1         ==> Hemisphere
					 > 1         ==> Hemelipsoid of revolution about minor axis
				Element.ConeHalfAngle = Half-angle of cone for cones or revolution or axicons
				Element.CurvOfRev = Curvature of revolution

		Output - FXYZ = Surface equation
				 DFXYZ[3] = Derivatives of surface equation
				 ErrorFlag = Error Flag
							 = 0  ==> no errors
							 > 0  ==> interpolation error
	}*/

	// int i=0,k=0;
	int i = 0;
	double X = 0.0, Y = 0.0, Z = 0.0;
	// double Rho=0.0, Rho2=0.0, Rho2i=0.0;
	double Rho = 0.0, Rho2 = 0.0;
	double DFDX = 0, DFDY = 0, DFDZ = 0;
	double Sum1 = 0.0, Sum2 = 0.0, Term = 0.0;
	double Y2 = 0.0, Y2J = 0.0;
	double FY = 0.0, ZZ = 0.0;
	double zm = 0.0, zr = 0.0;
	double dzrdx = 0.0, dzrdy = 0.0, delzx = 0.0, delzy = 0.0;
	double density = 0.0, delta = 0.0;
	double zx = 0.0, zy = 0.0;
	double dzdRho = 0.0, dRhodx = 0.0, dRhody = 0.0;

	// label 100, 130,160, 850, 990;

	// Initialize variables
	X = PosXYZ[0];
	Y = PosXYZ[1];
	Z = PosXYZ[2];
	*ErrorFlag = 0;

	//===SurfaceType = 1, 7  Rotationally Symmetric surfaces and single axis curvature sections===========================
	if (Element->SurfaceType == 1 || Element->SurfaceType == 7)
	{
		if (Element->SurfaceType == 1)
			Rho2 = X * X + Y * Y; // rotationally symmetric
		else
			Rho2 = X * X; // single axis curvature depends only on x

		if (Element->ConeHalfAngle != 0.0)
			goto Label_160;

		// wendelin 5-18-11 changes to allow different vertex curvature in the x and y directions for the parabola; this block of code
		// is a subset of the more general form below therefore it has been commented out.  It also assumes VertexCurvY = either VertexCurvX or zero
		// and doesn't allow different nonzero values for the parabolic case.   Not using the alpha parameters for the general case for now.
		/*
			for (i=0;i<5;i++)
			  if (Element->Alpha[i] != 0.0)
				goto Label_130;

			*FXYZ = Z - 0.5*Element->VertexCurvX*(Rho2 + Element->Kappa*Z*Z);
			DFDX = -Element->VertexCurvX*X;
			DFDY = -Element->VertexCurvY*Y; //VertexCurvY = VertexCurvX if rotationally symmetric or 0 if single axis curved
			DFDZ = 1.0 - Element->Kappa*Element->VertexCurvX*Z;
			goto Label_990;
		*/
		Sum1 = 0.0;
		Sum2 = 0.0;
		// Rho2i = 1.0;

		// wendelin 5-18-11
		/*
		for (i=0;i<5;i++)
		{
		  Sum1 = i*Element->Alpha[i]*Rho2i + Sum1;
		  Rho2i = Rho2i*Rho2;
		  Sum2 = Element->Alpha[i]*Rho2i + Sum2;
		}*/

		// wendelin 5-18-11 changes to allow different vertex curvature in the x and y directions for the parabola only
		//  Term = sqrt(1.0 - Element->Kappa*Element->VertexCurvX*Element->VertexCurvX*Rho2);

		if (1.0 - Element->Kappa * (Element->VertexCurvX * Element->VertexCurvX * X * X + Element->VertexCurvY * Element->VertexCurvY * Y * Y) < 0)
		{
			*ErrorFlag = 1; // Surface is not defined at this x,y location
		}

		Term = sqrt(1.0 - Element->Kappa * (Element->VertexCurvX * Element->VertexCurvX * X * X + Element->VertexCurvY * Element->VertexCurvY * Y * Y)); // new
		//*FXYZ = Z - Element->VertexCurvX*Rho2/(1.0 + Term) - Sum2;
		*FXYZ = Z - (Element->VertexCurvX * X * X + Element->VertexCurvY * Y * Y) / (1.0 + Term) - Sum2; // new

		DFDX = -X * (Element->VertexCurvX / Term + 2.0 * Sum1);
		DFDY = -Y * (Element->VertexCurvY / Term + 2.0 * Sum1); // VertexCurvY = VertexCurvX if rotationally symmetric or 0 if single axis curved
		DFDZ = 1.0;
		goto Label_990;

	Label_160:
		*FXYZ = Z - sqrt(Rho2) / tan(Element->ConeHalfAngle * (ACOSM1O180));
		DFDX = -X / (sqrt(Rho2) * tan(Element->ConeHalfAngle * (ACOSM1O180)));
		DFDY = -Y / (sqrt(Rho2) * tan(Element->ConeHalfAngle * (ACOSM1O180)));
		DFDZ = 1.0;
		goto Label_990;
	}

	//===SurfaceType = 2, Toroidal or Cylindrical surfaces========================== //not currently used
	if (Element->SurfaceType == 2)
	{
		Sum1 = 0.0;
		Sum2 = 0.0;
		Y2 = Y * Y;
		Y2J = 1.0;

		for (i = 0; i < 5; i++)
		{
			Sum1 = i * Element->Alpha[i] * Y2J * Y + Sum1;
			Y2J = Y2J * Y2;
			Sum2 = Element->Alpha[i] * Y2J + Sum2;
		}

		Term = sqrt(1.0 - Element->Kappa * Element->VertexCurvX * Element->VertexCurvX * Y2);
		FY = Element->VertexCurvX * Y2 / (1.0 + Term) + Sum2;
		*FXYZ = Z - FY - 0.5 * Element->CurvOfRev * (X * X + Z * Z - FY * FY);
		DFDX = -Element->CurvOfRev * X;
		DFDY = (Element->CurvOfRev * FY - 1.0) * (Element->VertexCurvX * Y / Term + 2.0 * Sum1);
		DFDZ = 1.0 - Element->CurvOfRev * Z;
		goto Label_990;
	}

	//===SurfaceType = 3, Plane Surfaces============================================
	/*     {The equation of a plane is: kx + ly + mz = p,  where k,l,m are the direction
		 cosines of the normal to the plane, and p is the distance from the origin
		 to the plane.  In this case, these parameters are contained in the Alpha array.}*/
	if (Element->SurfaceType == 3)
	{
		DFDX = Element->Alpha[0];
		DFDY = Element->Alpha[1];
		DFDZ = Element->Alpha[2];
		*FXYZ = DFDX * X + DFDY * Y + DFDZ * Z - Element->Alpha[3];
		goto Label_990;
	}

	// //===SurfaceType = 4, Surface specified by finite element data==================
	//   if (Element->SurfaceType == 4)
	//   {
	//     Rho2 = X*X + Y*Y;
	//     if (Rho2 == 0.0)
	//     {
	//       //FXYZ := Z - ZA[1];  ZA not defined yet
	//       *FXYZ = Z;
	//       DFDX = 0.0;
	//       DFDY = 0.0;
	//       DFDZ = 1;
	//       goto Label_990;
	//     }

	//     //Interpolate to find the z
	//     density = Element->FEData.nodes.size()/Element->ApertureArea;
	//     delta = 0.001/sqrt(density);
	//     FEInterpKD(X, Y, &Element->FEData, delta, &zr, &dzrdx, &dzrdy);

	//     PosXYZ[2] = zr;
	//     *FXYZ = Z - zr;
	//     DFDX = dzrdx;
	//     DFDY = dzrdy;
	//     DFDZ = 1.0;
	//     goto Label_990;
	//   }

	// //===SurfaceType = 5, VSHOT data================================================
	//   if (Element->SurfaceType == 5)
	//   {
	//     Rho2 = X*X + Y*Y;
	//     if (Rho2 == 0.0)
	//     {
	//       *FXYZ = Z;
	//       DFDX = 0.0;
	//       DFDY = 0.0;
	//       DFDZ = 1.0;
	//       goto Label_990;
	//     }
	//     // evaluate z, dz/dx and dz/dy from the monomial fit at x,y
	//     EvalMono(X, Y, Element->BCoefficients, Element->FitOrder, 0.0, 0.0, &zm); //the 0.0's are values for DeltaX and DeltaY; **[need to look at this further]**

	//     //Interpolate to find the slope residuals
	//     density = Element->VSHOTData.nrows()/Element->ApertureArea;

	//     /*
	//     if (Element->ShapeIndex == 'l' || Element->ShapeIndex == 'L')       //interpolation scheme for single axis curvature surfaces
	//       VSHOTInterpolateNew(X, Y, density, Element->VSHOTData, Element->VSHOTData.nrows(), &delzx, &delzy);
	//     else
	//       VSHOTInterpolate(X, Y, density, Element->VSHOTData, Element->VSHOTData.nrows(), &delzx, &delzy);
	//     */

	//     ::VSHOTInterpolateModShepard(X, Y, density, Element->VSHOTData, Element->VSHOTData.nrows(), &delzx, &delzy, ErrorFlag);

	//     if ( *ErrorFlag != 0 ) return;

	//     //Evaluate "real" z (i.e. the best estimate for z comes from the monomial fit)
	//     zr = zm;

	//     //Now evaluate the slopes  -  what we want here is the measured slope which is the best value to use
	//     //dzrdx := dzmdx + delzx;     //fit slope + (fit slope - meas. slope) =  wrong value
	//     //dzrdy := dzmdy + delzy;

	//     //dzrdx := dzmdx - delzx;       //fit slope - (fit slope - meas. slope) = meas. slope  (this is what we want)
	//     //dzrdy := dzmdy - delzy;
	//     dzrdx = delzx;                //if VSHOTInterpolate returns interpolated measured slopes and not slope RESIDUALS
	//     dzrdy = delzy;                // These values are angles of the slope in radians. Need to convert to dimensionless dz/dy and dz/dx so take tangent of angle

	//     dzrdx = tan(dzrdx);
	//     dzrdy = tan(dzrdy);

	//     PosXYZ[2] = zr;
	//     *FXYZ = Z - zr;
	//     DFDX = dzrdx;
	//     DFDY = dzrdy;
	//     //change sign of derivatives to agree with SurfaceType = 1
	//     DFDX = -DFDX;
	//     DFDY = -DFDY;
	//     DFDZ = 1.0;
	//     goto Label_990;
	//   }

	// //===SurfaceType = 6, Zernike monomials=========================================
	//   if (Element->SurfaceType == 6)
	//   {
	//     // k = Element->FitOrder + 1;
	//     ZZ = 0.0;
	//     DFDX = 0.0;
	//     DFDY = 0.0;

	//     // evaluate z from the monomial expression at x,y
	//     EvalMono(X, Y, Element->BCoefficients, Element->FitOrder, 0.0, 0.0, &ZZ); //the 0.0's are values for DeltaX and DeltaY; **[need to look at this further]**
	//     MonoSlope(Element->BCoefficients, Element->FitOrder, X, Y, &DFDX, &DFDY);

	//     PosXYZ[2] = ZZ;
	//     *FXYZ = Z - ZZ;
	//     //{change sign of derivatives to agree with SurfaceType = 1}
	//     DFDX = -DFDX;
	//     DFDY = -DFDY;
	//     DFDZ = 1.0;
	//     goto Label_990;
	//   }

	// //===SurfaceType = 8, rotationally symmetric polynomial surface=============================
	// if (Element->SurfaceType == 8)
	// {
	// 	ZZ = 0.0;
	// 	DFDX = 0.0;
	// 	DFDY = 0.0;

	// 	double yval = Y;
	// 	if (Element->ShapeIndex == 'l' || Element->ShapeIndex == 'L')
	// 		yval = 0.0;

	// 	// evaluate z & slopes from the polynomial expression at r = sqrt(x^2+y^2)
	// 	EvalPoly(X, yval, Element->PolyCoeffs, Element->FitOrder, &ZZ);
	// 	PolySlope(Element->PolyCoeffs, Element->FitOrder, X, yval, &DFDX, &DFDY);

	// 	PosXYZ[2] = ZZ;
	// 	*FXYZ = Z - ZZ;
	// 	//{change sign of derivatives to agree with SurfaceType = 1}
	// 	DFDX = -DFDX;
	// 	DFDY = -DFDY;
	// 	DFDZ = 1.0;
	// 	goto Label_990;
	// }

	// //===SurfaceType = 9, rotationally symmetric cubic spline interpolation surface==============
	// if (Element->SurfaceType == 9)
	// {
	// 	ZZ = 0.0;
	// 	DFDX = 0.0;
	// 	DFDY = 0.0;

	// 	Rho = sqrt(X * X + Y * Y);
	// 	dRhodx = X / Rho;
	// 	dRhody = Y / Rho;

	// 	if (Element->ShapeIndex == 'l' || Element->ShapeIndex == 'L') // x dimension only for single axis curvature
	// 	{
	// 		Rho = X;
	// 		dRhodx = 1.0;
	// 		dRhody = 0.0;
	// 	}

	// 	// evaluate z & slopes using cubic spline interpolation
	// 	if (!splint(Element->CubicSplineXData,
	// 				Element->CubicSplineYData,
	// 				Element->CubicSplineY2Data,
	// 				Element->CubicSplineXData.size(),
	// 				Rho, &ZZ, &dzdRho))
	// 	{
	// 		*ErrorFlag = 3;
	// 		return;
	// 	}

	// 	DFDX = dzdRho * dRhodx;
	// 	DFDY = dzdRho * dRhody;

	// 	PosXYZ[2] = ZZ;
	// 	*FXYZ = Z - ZZ;
	// 	//{change sign of derivatives to agree with SurfaceType = 1}
	// 	DFDX = -DFDX;
	// 	DFDY = -DFDY;
	// 	DFDZ = 1.0;
	// 	goto Label_990;
	// }

	// the following surfacetype is now handled above in the general case

	//===SurfaceType = 7, single axis curvature parabolic or spherical surface=============================
	/*{if SurfaceType = 7 then
	begin
	  if (SurfaceIndex = 'p') or (SurfaceIndex = 'P') then
	  begin
		 FXYZ := Z - X*X*VertexCurvX/2.0;
		 DFDX := -X*VertexCurvX;
		 DFDY := 0.0;
		 DFDZ := 1.0;
	  end;
	  if (SurfaceIndex = 's') or (SurfaceIndex = 'S') then
	  begin
	   FXYZ := Z - 0.5*VertexCurvX*(X*X + Z*Z);
	   DFDX := -VertexCurvX*X;
	   DFDY := 0.0;
	   DFDZ := 1.0 - VertexCurvX*Z;
	  end;
	end;}*/

//{****Offload derivatives of the surface equation}
Label_990:
	DFXYZ[0] = DFDX;
	DFXYZ[1] = DFDY;
	DFXYZ[2] = DFDZ;
}
// end of procedure--------------------------------------------------------------

bool splint(std::vector<double> &xa,
			std::vector<double> &ya,
			std::vector<double> &y2a,
			int n,
			double x,
			double *y,
			double *dydx)
{
	int klo = 0, khi = 0, k = 0;
	double h = 0.0, b = 0.0, a = 0.0;

	klo = 0;
	khi = n - 1;
	while (khi - klo > 1)
	{
		k = (khi + klo) / 2;
		if (xa[k] > x)
			khi = k;
		else
			klo = k;
	}

	h = xa[khi] - xa[klo];
	if (h != 0.0)
	{

		a = (xa[khi] - x) / h;
		b = (x - xa[klo]) / h;
		*y = a * ya[klo] + b * ya[khi] +
			 ((a * a * a - a) * y2a[klo] + (b * b * b - b) * y2a[khi]) * (h * h) / 6.0;
		*dydx = (ya[khi] - ya[klo]) / (xa[khi] - xa[klo]) -
				(3.0 * a * a - 1.0) * (xa[khi] - xa[klo]) * y2a[klo] / 6.0 + (3.0 * b * b - 1.0) * (xa[khi] - xa[klo]) * y2a[khi] / 6.0;
	}
	else
	{
		return false;
	}

	return true;
}

void TorusClosedForm(
	TElement *Element,
	double PosLoc[3],
	double CosLoc[3],
	double PosXYZ[3],
	double DFXYZ[3],
	double *PathLength,
	int *ErrorFlag)
{
	double Xo = 0.0, Yo = 0.0, Zo = 0.0, Epsilon = 0.0, Eta = 0.0, Rho = 0.0, Rs = 0.0, Ra = 0.0, X = 0.0, Y = 0.0, Z = 0.0, Fx = 0.0, Fy = 0.0, Fz = 0.0;
	double amatrix[5][5];
	double rvector[5];
	double imagroot1 = 0.0, imagroot2 = 0.0;
	int nn = 0;

	Rs = Element->CrossSectionRadius;
	Ra = Element->AnnularRadius;
	Xo = PosLoc[0];
	Yo = PosLoc[1];
	Zo = PosLoc[2];
	Epsilon = CosLoc[0];
	Eta = CosLoc[1];
	Rho = CosLoc[2];
	nn = 4;
	*ErrorFlag = 0;

	for (int i = 0; i < 5; i++)
	{
		rvector[i] = 0.0;
		for (int j = 0; j < 5; j++)
			amatrix[i][j] = 0.0;
	}

	amatrix[nn][4] = pow(Epsilon, 4) + 2.0 * Epsilon * Epsilon * (Eta * Eta + Rho * Rho) +
					 pow(Eta, 4) + 2.0 * Eta * Eta * Rho * Rho + pow(Rho, 4);

	amatrix[nn][3] = 4.0 * (Epsilon * Epsilon + Eta * Eta + Rho * Rho) * (Epsilon * Xo + Eta * Yo + Rho * Zo - Rho * Rs);

	amatrix[nn][2] = Xo * Xo * (6.0 * Epsilon * Epsilon + 2.0 * Eta * Eta + 2.0 * Rho * Rho) +
					 8.0 * Epsilon * Xo * (Eta * Yo + Rho * Zo - Rho * Rs) +
					 2.0 * Yo * Yo * (Epsilon * Epsilon + 3.0 * Eta * Eta + Rho * Rho) +
					 8.0 * Eta * Rho * Yo * (Zo - Rs) +
					 (Epsilon * Epsilon + Eta * Eta + 3.0 * Rho * Rho) * (2.0 * Zo * Zo - 4.0 * Rs * Zo) -
					 2.0 * Ra * Ra * (Epsilon * Epsilon + Eta * Eta - Rho * Rho) + 4.0 * Rho * Rho * Rs * Rs;

	amatrix[nn][1] = 4.0 * (Xo * Xo * (Epsilon * Xo + Eta * Yo + Rho * Zo - Rho * Rs) + Yo * Yo * (Epsilon * Xo + Eta * Yo + Rho * Zo - Rho * Rs) + Zo * Zo * (Epsilon * Xo + Eta * Yo + Rho * Zo - 3.0 * Rho * Rs) -
							2.0 * Epsilon * Rs * Xo * Zo - Epsilon * Ra * Ra * Xo - 2.0 * Eta * Rs * Yo * Zo -
							Eta * Ra * Ra * Yo + Rho * Ra * Ra * (Zo - Rs) + 2.0 * Rho * Rs * Rs * Zo);

	amatrix[nn][0] = pow(Xo, 4) + 2.0 * Xo * Xo * (Yo * Yo + Zo * Zo - 2.0 * Rs * Zo - Ra * Ra) +
					 pow(Yo, 4) + 2.0 * Yo * Yo * (Zo * Zo - 2.0 * Rs * Zo - Ra * Ra) +
					 pow(Zo, 4) - 4.0 * Rs * Zo * Zo * Zo + 2.0 * Ra * Ra * Zo * Zo + 4.0 * Rs * Rs * Zo * Zo - 4.0 * Ra * Ra * Rs * Zo + pow(Ra, 4);

	Root_432(nn, amatrix, rvector, &imagroot1, &imagroot2);

	// {pass results}
	if (imagroot1 == 0.0 && imagroot2 == 0.0)
	{
		// four real roots rvector[1-4] which are the pathlengths
		piksrt(4, rvector);	   // sort them
		if (rvector[4] <= 0.0) // ray heading away from surface
		{
			*PathLength = rvector[4];
			*ErrorFlag = 1;
			return;
		}
		//*PathLength = rvector[4];
		// if (rvector[3] <= 0.0 )
		//	goto Label_10;
		// else
		//	*PathLength = rvector[3];
		//
		// if (rvector[2] <= 0.0 )
		//	goto Label_10;
		// else
		//	*PathLength = rvector[2];
		//
		// if (rvector[1] <= 0.0 )
		//	goto Label_10;
		// else
		//	*PathLength = rvector[1];

		if (rvector[3] > 0)
		{
			*PathLength = rvector[3];
			if (rvector[2] > 0)
			{
				*PathLength = rvector[2];
				if (rvector[1] > 0)
				{
					*PathLength = rvector[1];
				}
			}
		}

		// goto Label_10;
	}

	else if (imagroot1 == 0.0 && imagroot2 != 0.0)
	{
		// two real roots rvector[1-2] which are the pathlengths
		if (rvector[1] <= rvector[2])
			*PathLength = rvector[1];
		else
			*PathLength = rvector[2];
		// goto Label_10;
	}

	else if (imagroot1 != 0.0 && imagroot2 == 0.0)
	{
		// two real roots rvector[3-4] which are the pathlengths
		if (rvector[3] <= rvector[4])
			*PathLength = rvector[3];
		else
			*PathLength = rvector[4];

		// goto Label_10;
	}

	else
	{
		*PathLength = 0.0;
	}

	// Label_10:
	if (*PathLength == 0.0) // ray missed torus completely
	{
		*ErrorFlag = 1;
		return;
	}

	X = Xo + *PathLength * Epsilon;
	Y = Yo + *PathLength * Eta;
	Z = Zo + *PathLength * Rho;
	Fx = -2.0 * X * (Ra - sqrt(X * X + Y * Y)) / sqrt(X * X + Y * Y);
	Fy = -2.0 * Y * (Ra - sqrt(X * X + Y * Y)) / sqrt(X * X + Y * Y);
	Fz = 2.0 * (Z - Rs);
	PosXYZ[0] = X;
	PosXYZ[1] = Y;
	PosXYZ[2] = Z;
	DFXYZ[0] = -Fx;
	DFXYZ[1] = -Fy;
	DFXYZ[2] = -Fz;
}

void QuadricSurfaceClosedForm(
	TElement *Element,
	double PosLoc[3],
	double CosLoc[3],
	double PosXYZ[3],
	double DFXYZ[3],
	double *PathLength,
	int *ErrorFlag)
{
	double Xdelta = 0.0, Ydelta = 0.0, Zdelta = 0.0;
	double Xc = 0, Yc = 0, Zc = 0, Kx = 0, Ky = 0, Kz = 0;
	double r = 0.0, r2 = 0.0, a2 = 0, b2 = 0, c2 = 0;
	double t1 = 0.0, t2 = 0.0, A = 0, B = 0, C = 0, slopemag = 0.0;

	*ErrorFlag = 0;

	switch (Element->SurfaceIndex)
	{
	case 's':
	case 'S': // sphere
		a2 = 1;
		b2 = 1;
		c2 = 1;
		Kx = 1;
		if (Element->SurfaceType == 7) // Single-axis curvature aperture
			Ky = 0;
		else
			Ky = 1;
		Kz = 1;
		r = 1.0 / Element->VertexCurvX;
		r2 = r * r;
		Xc = 0.0;
		Yc = 0.0;
		Zc = r;

		Xdelta = PosLoc[0] - Xc;
		Ydelta = PosLoc[1] - Yc;
		Zdelta = PosLoc[2] - Zc;

		A = CosLoc[0] * CosLoc[0] * Kx / a2 + CosLoc[1] * CosLoc[1] * Ky / b2 + CosLoc[2] * CosLoc[2] * Kz / c2;
		B = 2.0 * (Kx * Xdelta * CosLoc[0] / a2 + Ky * Ydelta * CosLoc[1] / b2 + Kz * Zdelta * CosLoc[2] / c2);
		C = Kx * Xdelta * Xdelta / a2 + Ky * Ydelta * Ydelta / b2 + Kz * Zdelta * Zdelta / c2 - r2;
		break;

	case 'p':
	case 'P': // parabola
		a2 = 2.0 / Element->VertexCurvX;
		b2 = Element->VertexCurvY > 0.0 ? 2.0 / Element->VertexCurvY : 1e10;
		c2 = 1.0;

		Kx = 1;
		if (Element->SurfaceType == 7) // Single-axis curvature aperture
			Ky = 0;
		else
			Ky = 1;
		Kz = 0;

		Xc = 0.0;
		Yc = 0.0;
		Zc = 0.0;

		Xdelta = PosLoc[0] - Xc;
		Ydelta = PosLoc[1] - Yc;
		Zdelta = PosLoc[2] - Zc;

		A = CosLoc[0] * CosLoc[0] * Kx / a2 + CosLoc[1] * CosLoc[1] * Ky / b2; // Note A can be zero
		B = 2.0 * (Kx * Xdelta * CosLoc[0] / a2 + Ky * Ydelta * CosLoc[1] / b2) - CosLoc[2] / c2;
		C = Kx * Xdelta * Xdelta / a2 + Ky * Ydelta * Ydelta / b2 - Zdelta / c2;

		break;

	case 'o':
	case 'O': // other
		a2 = 1;
		b2 = 1;
		c2 = 1;
		Kx = 1;
		Ky = 1;
		Kz = Element->Kappa;
		Xc = 0.0;
		Yc = 0.0;
		Zc = 1.0 / Element->Kappa / Element->VertexCurvX; // VertexCurvX = VertexCurvY for this surface type

		Xdelta = PosLoc[0] - Xc;
		Ydelta = PosLoc[1] - Yc;
		Zdelta = PosLoc[2] - Zc;

		A = CosLoc[0] * CosLoc[0] * Kx / a2 + CosLoc[1] * CosLoc[1] * Ky / b2 + CosLoc[2] * CosLoc[2] * Kz / c2;
		B = 2.0 * (Kx * Xdelta * CosLoc[0] / a2 + Ky * Ydelta * CosLoc[1] / b2 + Kz * Zdelta * CosLoc[2] / c2);
		C = Kx * Xdelta * Xdelta / a2 + Ky * Ydelta * Ydelta / b2 + Kz * Zdelta * Zdelta / c2 - 1.0 / Element->Kappa / (Element->VertexCurvX * Element->VertexCurvX);
		break;

	case 't':
	case 'T': // cylinder
		a2 = 1;
		b2 = 1;
		c2 = 1;
		Kx = 1;
		Ky = 0;
		Kz = 1;
		r = 1.0 / Element->CurvOfRev;
		r2 = r * r;
		Xc = 0.0;
		Yc = 0.0;
		Zc = r;

		Xdelta = PosLoc[0] - Xc;
		Ydelta = PosLoc[1] - Yc;
		Zdelta = PosLoc[2] - Zc;

		A = CosLoc[0] * CosLoc[0] * Kx / a2 + CosLoc[1] * CosLoc[1] * Ky / b2 + CosLoc[2] * CosLoc[2] * Kz / c2;
		B = 2.0 * (Kx * Xdelta * CosLoc[0] / a2 + Ky * Ydelta * CosLoc[1] / b2 + Kz * Zdelta * CosLoc[2] / c2);
		C = Kx * Xdelta * Xdelta / a2 + Ky * Ydelta * Ydelta / a2 + Kz * Zdelta * Zdelta / c2 - r2;
		break;

	case 'c':
	case 'C': // cone
		break;

	case 'f':
	case 'F': // flat
		break;
	}

	if (fabs(A) < 1e-12) // Should only happen for parabolas
	{
		t1 = -C / B;
		if (t1 > 0)
		{
			PosXYZ[0] = PosLoc[0] + t1 * CosLoc[0];
			PosXYZ[1] = PosLoc[1] + t1 * CosLoc[1];
			PosXYZ[2] = PosLoc[2] + t1 * CosLoc[2];
			*PathLength = t1;
			// goto Label_100;
		}
		else
		{
			*PathLength = 0.0; // ray tangent or missed
			*ErrorFlag = 1;
			return;
		}
	}
	else if (sqr(B) > 4.0 * A * C)
	{
		t1 = (-B + sqrt(sqr(B) - 4.0 * A * C)) / (2.0 * A);
		t2 = (-B - sqrt(sqr(B) - 4.0 * A * C)) / (2.0 * A);
		if (t2 > 0) // initial ray location outside surface
		{
			PosXYZ[0] = PosLoc[0] + t2 * CosLoc[0];
			PosXYZ[1] = PosLoc[1] + t2 * CosLoc[1];
			PosXYZ[2] = PosLoc[2] + t2 * CosLoc[2];
			*PathLength = t2;

			//*************************************************************************************************************
			// makes sure to get shortest ray path on valid side of surface; 10-05-10    for open surface of parabola
			// if cylinder, then PosXYZ[3] will always be less than or equal to Element.Zaperture so never passes this test.
			// Test for  cylinder follows below.
			if (PosXYZ[2] > Element->ZAperture)
			{
				PosXYZ[0] = PosLoc[0] + t1 * CosLoc[0];
				PosXYZ[1] = PosLoc[1] + t1 * CosLoc[1];
				PosXYZ[2] = PosLoc[2] + t1 * CosLoc[2];
				*PathLength = t1;
			}

			// Remember at this point, intersection is being found on an INFINITELY long cylinder.
			// if 1st intersection on INFINITELY long cylinder is from the outside, t2, check to make sure
			// intersection is within the finite
			// length of the actual cylinder geometry, if not then 2nd intersection on the inside, t1,
			// is valid one to use.  This means ray could
			// enter from the open end  of the cylinder and hit on the inside.  The final test for this is in the
			// calling routine:  DetermineElementIntersectionNew
			// Wendelin 10-05-10
			if ((Element->SurfaceIndex == 't') || (Element->SurfaceIndex == 'T'))
			{
				if ((PosXYZ[1] < -Element->ParameterC / 2.0) || (PosXYZ[1] > Element->ParameterC / 2.0))
				{
					PosXYZ[0] = PosLoc[0] + t1 * CosLoc[0];
					PosXYZ[1] = PosLoc[1] + t1 * CosLoc[1];
					PosXYZ[2] = PosLoc[2] + t1 * CosLoc[2];
					*PathLength = t1;
				}
			}

			// Partial cylinder (sphere with single-axis curvature) needs the same check as cylinder.
			// Two intersections are possible, check if the first intersection along the ray path occurs within the length bounds and, if not, return the second intersection
			// The final test for a positive ray path and a valid intersection location is in DetermineElementIntersectionNew. If t1 is negative, this intersection location will be ignored in DetermineElementIntersectionNew
			if (((Element->SurfaceIndex == 's') || (Element->SurfaceIndex == 'S')) && (Element->SurfaceType == 7))
			{
				if ((PosXYZ[1] < -Element->ParameterC / 2.0) || (PosXYZ[1] > Element->ParameterC / 2.0))
				{
					PosXYZ[0] = PosLoc[0] + t1 * CosLoc[0];
					PosXYZ[1] = PosLoc[1] + t1 * CosLoc[1];
					PosXYZ[2] = PosLoc[2] + t1 * CosLoc[2];
					*PathLength = t1;
				}
			}
			//***********************************************************************************************************

			// goto Label_100;
		}
		else if (t2 == 0) // initial ray location at surface
		{
			PosXYZ[0] = PosLoc[0] + t1 * CosLoc[0];
			PosXYZ[1] = PosLoc[1] + t1 * CosLoc[1];
			PosXYZ[2] = PosLoc[2] + t1 * CosLoc[2];
			*PathLength = t1;
			// goto Label_100;
		}
		else if (t2 < 0 && t1 > 0) // initial ray location inside surface
		{
			PosXYZ[0] = PosLoc[0] + t1 * CosLoc[0];
			PosXYZ[1] = PosLoc[1] + t1 * CosLoc[1];
			PosXYZ[2] = PosLoc[2] + t1 * CosLoc[2];
			*PathLength = t1;
			// goto Label_100;
		}
		else if (t1 <= 0)
		{
			*PathLength = t1; // ray heading away from surface
			*ErrorFlag = 1;
			return;
		}
	}
	else
	{
		*PathLength = 0.0; // ray tangent or missed
		*ErrorFlag = 1;
		return;
	}

	if (Element->SurfaceIndex == 'p' || Element->SurfaceIndex == 'P')
	{
		slopemag = sqrt(sqr(2.0 * Kx * (PosXYZ[0] - Xc) / a2) + sqr(2.0 * Ky * (PosXYZ[1] - Yc) / b2) + 1.0);
		DFXYZ[0] = -(2.0 * Kx * (PosXYZ[0] - Xc) / a2) / slopemag;
		DFXYZ[1] = -(2.0 * Ky * (PosXYZ[1] - Yc) / b2) / slopemag;
		DFXYZ[2] = 1.0 / slopemag;
	}
	else
	{
		slopemag = sqrt(sqr(2.0 * Kx * (PosXYZ[0] - Xc) / a2) + sqr(2.0 * Ky * (PosXYZ[1] - Yc) / b2) + sqr(2.0 * Kz * (PosXYZ[2] - Zc) / c2));
		DFXYZ[0] = -(2.0 * Kx * (PosXYZ[0] - Xc) / a2) / slopemag;
		DFXYZ[1] = -(2.0 * Ky * (PosXYZ[1] - Yc) / b2) / slopemag;
		DFXYZ[2] = -(2.0 * Kz * (PosXYZ[2] - Zc) / c2) / slopemag;
	}
}

void SurfaceZatXYPair(
	double PosXYZ[3],
	TElement *Element,
	double *FXYZ,
	int *ErrorFlag)
{
	/*{Purpose: To compute the Z value of the surface equation at an X,Y pair.
		Input - PosXYZ[3] = X, Y, Z coordinate position
				Element.SurfaceType = Surface type flag
							  = 1 for rotationally symmetric surfaces
							  = 2 for torics and cylinders
							  = 3 for plane surfaces
							  = 4 for surface interpolated from finite element data points
							  = 5 for surface interpolated from VSHOT data points
							  = 6 for surface described by Zernike monomials
							  = 7 for single axis parabolic curvature surfaces
							  = 8 for rotationally symmetric polynomial description
							  = 9 for       "          "     cubic spline interpolation
				Element.Alpha = Sensitivity coefficients which specify deviation from conic
						of revolution
				Element.VertexCurvX = Vertex Curvature of surface
				Element.Kappa = Surface specifier
					 < 0         ==> Hyperboloid
					 = 0         ==> Paraboloid
					 > 0 and < 1 ==> Hemelipsoid of revolution about major axis
					 = 1         ==> Hemisphere
					 > 1         ==> Hemelipsoid of revolution about minor axis
				Element.ConeHalfAngle = Half-angle of cone for cones or revolution or axicons
				Element.CurvOfRev = Curvature of revolution

		Output - FXYZ = Z value of Surface equation
				 ErrorFlag = Error Flag
							 = 0  ==> no errors
							 > 0  ==> interpolation error
	}*/
	int i = 0;
	// double X=0.0,Y=0.0,Z=0.0;
	double X = 0.0, Y = 0.0;
	double Rho2 = 0.0, Rho = 0.0;
	double Sum1 = 0.0, ZZ = 0.0, zm = 0.0;

	// Initialize variables
	X = PosXYZ[0];
	Y = PosXYZ[1];
	// Z = PosXYZ[2];
	*ErrorFlag = 0;

	//===SurfaceType = 1, 7  Rotationally Symmetric surfaces and single axis curvature sections===========================
	if (Element->SurfaceType == 1 || Element->SurfaceType == 7)
	{
		if (Element->SurfaceType == 1)
			Rho2 = X * X + Y * Y; // rotationally symmetric
		else
			Rho2 = X * X; // single axis curvature depends only on x

		Rho = sqrt(Rho2);

		if (Element->ConeHalfAngle != 0.0)
			goto Label_160;

		// wendelin 5-18-11

		// if (Element->Kappa*Element->VertexCurvX*Element->VertexCurvX*Rho2 > 1.0)  //xy pair cannot be found on closed surface   06-10-07
		if (Element->Kappa * (Element->VertexCurvX * Element->VertexCurvX * X * X + Element->VertexCurvY * Element->VertexCurvY * Y * Y) > 1.0) // xy pair cannot be found on closed surface   06-10-07
		{
			*FXYZ = 0.0;
			return;
		}

		// wendelin 5-18-11
		//  *FXYZ = Element->VertexCurvX*Rho2/(1.0+sqrt(1.0-Element->Kappa*Element->VertexCurvX*Element->VertexCurvX*Rho2));
		*FXYZ = (Element->VertexCurvX * X * X + Element->VertexCurvY * Y * Y) / (1.0 + sqrt(1.0 - Element->Kappa * (Element->VertexCurvX * Element->VertexCurvX * X * X + Element->VertexCurvY * Element->VertexCurvY * Y * Y)));

		/*        for (i=0;i<5;i++)
					 if (Element->Alpha[i] != 0.0) goto Label_130;
			   */

		return;

		Sum1 = 0.0;
		for (i = 0; i < 5; i++)
			Sum1 = Element->Alpha[i] * pow(Rho, 2 * (i + 1)) + Sum1;

		*FXYZ += Sum1;
		return;

	Label_160:
		*FXYZ = sqrt(Rho2) / tan(Element->ConeHalfAngle * (ACOSM1O180));
		return;
	}

	//===SurfaceType = 3, Plane Surfaces============================================
	/* {The equation of a plane is: kx + ly + mz = p,  where k,l,m are the direction
	 cosines of the normal to the plane, and p is the distance from the origin
	 to the plane.  In this case, these parameters are contained in the Alpha array.}
	 {if SurfaceType = 3 then
	 begin
		DFDX = Alpha[1];
		DFDY = Alpha[2];
		DFDZ = Alpha[3];
		FXYZ = DFDX*X + DFDY*Y + DFDZ*Z - Alpha[4];
		return;
	 end;}*/

	//===SurfaceType = 4, Surface specified by finite element data==================
	/*{if SurfaceType = 4 then
	 begin
		Rho2 = X*X + Y*Y;
		if Rho2 = 0.0 then
		begin
			 //FXYZ = Z - ZA[1];  ZA not defined yet
			 FXYZ = Z;
			 DFDX = 0.0;
			 DFDY = 0.0;
			 DFDZ = 1;
			 return;
		end;
		  //Interpolate to find the z
		  Density = FENumPoints/ApertureArea;
		  delta = 0.1/sqrt(density);
		  FEInterpNew(X, Y, Density, FEData, FENumPoints, zr);

		  //Now evaluate the slopes
		  FEInterpNew(X+delta, Y, Density, FEData, FENumPoints, zx);
		  FEInterpNew(X, Y+delta, Density, FEData, FENumPoints, zy);
		  dzrdx = (zx-zr)/delta;
		  dzrdy = (zy-zr)/delta;

		  PosXYZ[3] = zr;
		  FXYZ = z - zr;
		  DFDX = dzrdx;
		  DFDY = dzrdy;
		  //change sign of derivatives to agree with SurfaceType = 1
		  DFDX = -DFDX;
		  DFDY = -DFDY;
		  DFDZ = 1.0;
		  return;
	 end;}*/

	// //===SurfaceType = 5, VSHOT data================================================
	//   if (Element->SurfaceType == 5)
	//   {
	//     Rho2 = X*X + Y*Y;
	//     if (Rho2 == 0.0)
	//     {
	//       *FXYZ = 0.0;
	//       return;
	//     }
	//     // evaluate z, dz/dx and dz/dy from the monomial fit at x,y
	//     EvalMono(X, Y, Element->BCoefficients, Element->FitOrder, 0.0, 0.0, &zm); //the 0.0's are values for DeltaX and DeltaY; **[need to look at this further]**
	//     *FXYZ = zm;
	//     return;
	//   }

	// //===SurfaceType = 6, Zernike monomials=========================================
	//   if (Element->SurfaceType == 6)
	//   {
	//           // evaluate z from the monomial expression at x,y
	//     EvalMono(X, Y, Element->BCoefficients, Element->FitOrder, 0.0, 0.0, &ZZ); //the 0.0's are values for DeltaX and DeltaY; **[need to look at this further]**
	//     *FXYZ = ZZ;
	//     return;
	//   }

	// //===SurfaceType = 8, rotationally symmetric polynomial surface=============================
	//   if (Element->SurfaceType == 8)
	//   {
	//     // evaluate z & slopes from the polynomial expression at r = sqrt(x^2+y^2)

	//     double yval = Y;
	//     if ( Element->ShapeIndex == 'l' || Element->ShapeIndex == 'L' )
	//       yval = 0.0;

	//     EvalPoly(X, yval, Element->PolyCoeffs, Element->FitOrder, &ZZ);
	//     *FXYZ = ZZ;
	//     return;
	//   }
	//===SurfaceType = 9, rotationally symmetric cubic spline interpolation surface==============
	/*if (Element->SurfaceType == 9)
  {
   ZZ = 0.0;
   DFDX = 0.0;
   DFDY = 0.0;

   Rho = sqrt(X*X+Y*Y);
   dRhodx = X/Rho;
   dRhody = Y/Rho;
   //evaluate z & slopes using cubic spline interpolation
   splint(Element->CubicSplineXData,
	 Element->CubicSplineYData,
	 Element->CubicSplineY2Data,
	 Element->CubicSplineXData.length(),
	 Rho,
	 &ZZ,&dzdRho);

   DFDX = dzdRho*dRhodx;
   DFDY = dzdRho*dRhody;

   PosXYZ[2] = ZZ;
   *FXYZ = Z - ZZ;
   //change sign of derivatives to agree with SurfaceType = 1
   DFDX = -DFDX;
   DFDY = -DFDY;
   return;
  }*/

	// the following surfacetype is now handled above in the general case

	//===SurfaceType = 7, single axis curvature parabolic or spherical surface=============================
	/*{if SurfaceType = 7 then
	begin
	  if (SurfaceIndex = 'p') or (SurfaceIndex = 'P') then
	  begin
		 FXYZ = Z - X*X*VertexCurvX/2.0;
		 DFDX = -X*VertexCurvX;
		 DFDY = 0.0;
		 DFDZ = 1.0;
	  end;
	  if (SurfaceIndex = 's') or (SurfaceIndex = 'S') then
	  begin
	   FXYZ = Z - 0.5*VertexCurvX*(X*X + Z*Z);
	   DFDX = -VertexCurvX*X;
	   DFDY = 0.0;
	   DFDZ = 1.0 - VertexCurvX*Z;
	  end;
	end;}*/
}

void piksrt(int n, double arr[5])
{
	int i;
	for (int j = 1; j < n; j++)
	{
		double a = arr[j];
		for (i = j - 1; i >= 0; i--)
		{
			if (arr[i] <= a)
				goto Label_10;
			arr[i + 1] = arr[i];
		}
		i = 0;
	Label_10:
		arr[i + 1] = a;
	}
}
