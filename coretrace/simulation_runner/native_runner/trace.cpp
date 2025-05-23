
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
#include "element_intersection.hpp"
#include "native_runner_backend.hpp"
#include "native_runner_types.hpp"
#include "treemesh.hpp"

// #define   Order 3
// #define   NumIterations 20
// #define   Epsilon 0.000001

void FindElementHit(
	// stage info
	const int i,
	const TStage *Stage,
	const bool PT_override,
	const bool AsPowerTower,
	// element info
	const int nintelements,
	const std::vector<void *> &sunint_elements,
	const std::vector<void *> &reflint_elements,
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
			std::vector<void *> sunint_elements;

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
				std::vector<void *> reflint_elements;
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

