
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



#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <ctime>
//#define WITH_DEBUG_TIMER
#ifdef WITH_DEBUG_TIMER
    #include <chrono>    //comment out for production
#endif

#include "types.h"
#include "procs.h"
#include "treemesh.h"
//#include "raytrace.cpp"

void time_refactored(const char *message, ofstream *fout)
{
#ifdef WITH_DEBUG_TIMER
    (*fout) << message << chrono::duration_cast< chrono::milliseconds >( chrono::system_clock::now().time_since_epoch() ).count() << "\n"; 
#endif
}

inline void CopyVec3_refactored( double dest[3], const std::vector<double> &src )
{
	dest[0] = src[0];
	dest[1] = src[1];
	dest[2] = src[2];
}

inline void CopyVec3_refactored( std::vector<double> &dest, double src[3] )
{
	dest[0] = src[0];
	dest[1] = src[1];
	dest[2] = src[2];
}

inline void CopyVec3_refactored( double dest[3], double src[3] )
{
	dest[0] = src[0];
	dest[1] = src[1];
	dest[2] = src[2];
}

#define ZeroVec(x) x[0]=x[1]=x[2]=0.0

class GlobalRay_refactored
{
public:
	GlobalRay_refactored() {
		Num = 0;
		for (int i=0;i<3;i++) Pos[i]=Cos[i]=0.0;
	}

	double Pos[3];
	double Cos[3];
	st_uint_t Num;
};

//structure to store element address and projected polar coordinate size
struct eprojdat_refactored
{
    TElement* el_addr;
    double d_proj;
    double az;
    double zen;

    eprojdat_refactored(){};
    eprojdat_refactored(TElement* e, double d, double a, double z)
    {
        el_addr = e;
        d_proj = d;
        az = a;
        zen = z;
    };
};

//Comparison function for sorting vector of eprojdat
static bool eprojdat_compare_refactored(const eprojdat_refactored &A, const eprojdat_refactored &B)
{
    return A.d_proj > B.d_proj;
};

void FindElementHit(
	// stage info
	const int i, const TStage* Stage, const bool PT_override, const bool AsPowerTower,
	
	// element info
	const int nintelements, const vector<void*>& sunint_elements, const vector<void*>& reflint_elements,
	
	 // ray info
	const int RayNumber, const bool in_multi_hit_loop,
	double(&PosRayStage)[3], double(&CosRayStage)[3],


	double(&LastPosRaySurfElement)[3], double(&LastCosRaySurfElement)[3], double(&LastDFXYZ)[3],
	st_uint_t& LastElementNumber, st_uint_t& LastRayNumber, 
	double(&LastPosRaySurfStage)[3], double(&LastCosRaySurfStage)[3],

	int& ErrorFlag, int& LastHitBackSide,
	bool& StageHit)
{
	// Initialize Variables
	double LastPathLength = 1e99;
	int HitBackSide = 0;
	int InterceptFlag = 0;
	double DFXYZ[3] = { 0.0, 0.0, 0.0 };
	double PosRayElement[3] = { 0.0, 0.0, 0.0 };
	double CosRayElement[3] = { 0.0, 0.0, 0.0 };
	double PosRaySurfStage[3] = { 0.0, 0.0, 0.0 };
	double CosRaySurfStage[3] = { 0.0, 0.0, 0.0 };
	double PosRaySurfElement[3] = { 0.0, 0.0, 0.0 };
	double CosRaySurfElement[3] = { 0.0, 0.0, 0.0 };
	StageHit = false;


	for (st_uint_t j = 0; j < nintelements; j++)
	{
		TElement* Element; // = Stage->ElementList[j];
		if (i == 0 && !PT_override)
		{
			if (in_multi_hit_loop)
			{
				if (AsPowerTower)
					Element = (TElement*)reflint_elements.at(j);
				else
					Element = (TElement*)Stage->ElementList[j];
			}
			else
				Element = (TElement*)sunint_elements.at(j);
		}
		else
			Element = Stage->ElementList[j];

		if (!Element->Enabled)
			continue;

		//  {Transform ray to element[j] coord system of Stage[i]}
		TransformToLocal(PosRayStage, CosRayStage,
			Element->Origin, Element->RRefToLoc,
			PosRayElement, CosRayElement);

		ErrorFlag = 0;
		HitBackSide = 0;
		InterceptFlag = 0;
		double PathLength = 0;

		// increment position by tiny amount to get off the element if tracing to the same element
		PosRayElement[0] = PosRayElement[0] + 1.0e-5 * CosRayElement[0];
		PosRayElement[1] = PosRayElement[1] + 1.0e-5 * CosRayElement[1];
		PosRayElement[2] = PosRayElement[2] + 1.0e-5 * CosRayElement[2];

		// {Determine if ray intersects element[j]; if so, Find intersection point with surface of element[j] }
		DetermineElementIntersectionNew(Element, PosRayElement, CosRayElement,
			PosRaySurfElement, CosRaySurfElement, DFXYZ,
			&PathLength, &ErrorFlag, &InterceptFlag, &HitBackSide);



		if (InterceptFlag)
		{
			//{If hit multiple elements, this loop determines which one hit first.
			//Also makes sure that correct part of closed surface is hit. Also, handles wavy, but close to flat zernikes and polynomials correctly.}
			//if (PathLength < LastPathLength) and (PosRaySurfElement[2] <= Element->ZAperture) then
			if (PathLength < LastPathLength)
			{
				if (PosRaySurfElement[2] <= Element->ZAperture
					|| Element->SurfaceIndex == 'm'
					|| Element->SurfaceIndex == 'M'
					|| Element->SurfaceIndex == 'r'
					|| Element->SurfaceIndex == 'R')
				{
					StageHit = true;
					LastPathLength = PathLength;
					CopyVec3_refactored(LastPosRaySurfElement, PosRaySurfElement);
					CopyVec3_refactored(LastCosRaySurfElement, CosRaySurfElement);
					CopyVec3_refactored(LastDFXYZ, DFXYZ);
					LastElementNumber = (i == 0 && !PT_override) ? Element->element_number : j + 1;    //mjw change from j index to element id
					LastRayNumber = RayNumber;
					TransformToReference(PosRaySurfElement, CosRaySurfElement,
						Element->Origin, Element->RLocToRef,
						PosRaySurfStage, CosRaySurfStage);

					CopyVec3_refactored(LastPosRaySurfStage, PosRaySurfStage);
					CopyVec3_refactored(LastCosRaySurfStage, CosRaySurfStage);
					LastHitBackSide = HitBackSide;
				}
			}
		}
	}
}

void ProcessInteraction(
	// system info
	TSystem* System, MTRand &myrng, const bool IncludeSunShape, TOpticalProperties* optics,
	const bool IncludeErrors,

	// stage info
	const int i, const TStage* Stage, int k,
	
	// ray info
	const st_uint_t MultipleHitCount,
	double(&LastDFXYZ)[3],
	
	double(&LastCosRaySurfElement)[3], int& ErrorFlag,
	double(&CosRayOutElement)[3], double(&LastPosRaySurfElement)[3],
	double(&PosRayOutElement)[3], int& myrng_counter

)
{

	// Initialize
	double CosIn[3] = { 0.0, 0.0, 0.0 };
	double CosOut[3] = { 0.0, 0.0, 0.0 };

	

	if (!Stage->Virtual)
	{
		if (IncludeSunShape && i == 0 && MultipleHitCount == 1)//change to account for first hit only in primary stage 8-11-31
		{
			// Apply sunshape to UNPERTURBED ray at intersection point
			//only apply sunshape error once for primary stage
			CopyVec3_refactored(CosIn, LastCosRaySurfElement);
			Errors(myrng, CosIn, 1, &System->Sun,
				Stage->ElementList[k], optics, CosOut, LastDFXYZ);  //sun shape
			CopyVec3_refactored(LastCosRaySurfElement, CosOut);
		}

		//{Determine interaction at surface and direction of perturbed ray}
		ErrorFlag = 0;

		// {Apply surface normal errors to surface normal before interaction ray at intersection point - Wendelin 11-23-09}
		if (IncludeErrors)
		{
			CopyVec3_refactored(CosIn, CosRayOutElement);
			SurfaceNormalErrors(myrng, LastDFXYZ, optics, CosOut);  //surface normal errors
			myrng_counter++;
			CopyVec3_refactored(LastDFXYZ, CosOut);
		}

		Interaction(myrng, LastPosRaySurfElement, LastCosRaySurfElement, LastDFXYZ,
			Stage->ElementList[k]->InteractionType, optics, 630.0,
			PosRayOutElement, CosRayOutElement, &ErrorFlag);
		myrng_counter++;

		// {Apply specularity optical error to PERTURBED (i.e. after interaction) ray at intersection point}
		if (IncludeErrors)
		{
			if (optics->DistributionType == 'F' || optics->DistributionType == 'f')
				CopyVec3_refactored(CosIn, LastDFXYZ);  // Apply diffuse errors relative to surface normal
			else
				CopyVec3_refactored(CosIn, CosRayOutElement); // Apply all other errors relative to the specularly-reflected direction

			Errors(myrng, CosIn, 2, &System->Sun,
				Stage->ElementList[k], optics, CosOut, LastDFXYZ);  //optical errors
			myrng_counter++;
			CopyVec3_refactored(CosRayOutElement, CosOut);
		}
	}
}

bool Trace_refactored_scratch(TSystem* System, unsigned int seed,
	st_uint_t NumberOfRays,
	st_uint_t MaxNumberOfRays,
	bool IncludeSunShape,
	bool IncludeErrors,
	bool AsPowerTower,
	int (*callback)(st_uint_t ntracedtotal, st_uint_t ntraced, st_uint_t ntotrace, st_uint_t curstage, st_uint_t nstages, void* data),
	void* cbdata)
{
	// Skipping PT_override logic, st_data
	bool PT_override = true;
	AsPowerTower = false;

	// Check inputs

	// Initialize variables
	MTRand myrng(seed);
	int myrng_counter = 0;

	// Initialize Internal State Variables
	st_uint_t RayNumber = 1;						// Ray Number of current ray
	bool PreviousStageHasRays = false;
	st_uint_t LastRayNumberInPreviousStage = NumberOfRays;

	// Initialize Sun
	double PosSunStage[3] = { 0.0, 0.0, 0.0 };
	if (!SunToPrimaryStage(System, System->StageList[0], &System->Sun, PosSunStage))
		return false;

	// Define IncomingRays
	std::vector<GlobalRay_refactored> IncomingRays;	// Vector of rays from previous stage, going into next stage
	try
	{
		IncomingRays.resize(NumberOfRays);
	}
	catch (std::exception& e) {
		System->errlog("Incoming rays resize exception: %d, '%s'", NumberOfRays, e.what());
		return false;
	}

	// Skipping hash trees

	// Loop through stages
	for (st_uint_t i = 0; i < System->StageList.size(); i++)
	{
		// Skipping check to see if previous stage has rays

		// Get Current Stage
		TStage* Stage = System->StageList[i];

		// Initialize stage variables
		st_uint_t StageDataArrayIndex = 0;
		st_uint_t PreviousStageDataArrayIndex = 0;

		// Loop through rays
		bool StageHasRays = true;
		while (StageHasRays)
		{
			// Initialize Global Coordinates
			double PosRayGlob[3] = { 0.0, 0.0, 0.0 };
			double CosRayGlob[3] = { 0.0, 0.0, 0.0 };

			// Initialize Stage Coordinates
			double PosRayStage[3] = { 0.0, 0.0, 0.0 };
			double CosRayStage[3] = { 0.0, 0.0, 0.0 };

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

				// Skipping check, skipping hash optimizations
			}
			else
			{
				if (StageDataArrayIndex >= IncomingRays.size())
				{
					double asdg = 0;
				}

				// Get ray from previous stage
				RayNumber = IncomingRays[StageDataArrayIndex].Num;
				CopyVec3_refactored(PosRayGlob, IncomingRays[StageDataArrayIndex].Pos);
				CopyVec3_refactored(CosRayGlob, IncomingRays[StageDataArrayIndex].Cos);
				StageDataArrayIndex++;
			}

			// transform the global incoming ray to local stage coordinates
			TransformToLocal(PosRayGlob, CosRayGlob,
				Stage->Origin, Stage->RRefToLoc,
				PosRayStage, CosRayStage);

			// Skipping progress bar update

			// Initialize internal variables for ray intersection tracing
			std::vector<void*> emptyVector;
			bool RayInStage = true;
			bool in_multi_hit_loop = false;
			double LastPosRaySurfElement[3] = { 0.0, 0.0, 0.0 };
			double LastCosRaySurfElement[3] = { 0.0, 0.0, 0.0 };
			double LastPosRaySurfStage[3] = { 0.0, 0.0, 0.0 };
			double LastCosRaySurfStage[3] = { 0.0, 0.0, 0.0 };
			double LastDFXYZ[3] = { 0.0, 0.0, 0.0 };
			st_uint_t LastElementNumber = 0;
			st_uint_t LastRayNumber = 0;
			int ErrorFlag;
			int LastHitBackSide;
			bool StageHit;
			int MultipleHitCount = 0;
			double PosRayOutElement[3] = { 0.0, 0.0, 0.0 };
			double CosRayOutElement[3] = { 0.0, 0.0, 0.0 };

			// Start Loop to trace ray until it leaves stage
			bool RayIsAbsorbed = false;
			while (RayInStage)
			{
				// Set number of elements to search through
					// skipping PowerTower optimizations, checks for zero elements
				st_uint_t nintelements = Stage->ElementList.size();

				// Find the element the ray hits
				FindElementHit(i, Stage, PT_override, AsPowerTower,
					nintelements, emptyVector, emptyVector,
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
				TRayData::ray_t* p_ray = Stage->RayData.Append(LastPosRaySurfStage,
					LastCosRaySurfStage,
					LastElementNumber,
					i + 1,
					LastRayNumber);

				// Skipping check to make sure p_ray was generated successfully

				// Skipping LastElementNumber == 0 check

				// Increment MultipleHitCount
				MultipleHitCount++;

				// Get optics and check for absorption
				TOpticalProperties* optics = 0;
				if (Stage->Virtual)
				{
					// If stage is virtual, there is no interaction
					CopyVec3_refactored(PosRayOutElement, LastPosRaySurfElement);
					CopyVec3_refactored(CosRayOutElement, LastCosRaySurfElement);
				}
				else
				{
					// trace through the interaction
					TElement* optelm = Stage->ElementList[p_ray->element - 1];

					if (LastHitBackSide)
						optics = &optelm->Optics->Back;
					else
						optics = &optelm->Optics->Front;

					double TestValue;
					double UnitLastDFXYZ[3] = { 0.0, 0.0, 0.0 };
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
								IncidentAngle = acos(DOT(LastCosRaySurfElement, UnitLastDFXYZ)) * 1000.;  //[mrad]
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
								IncidentAngle = acos(DOT(LastCosRaySurfElement, UnitLastDFXYZ)) * 1000.;  //[mrad]
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
					//goto Label_EndStageLoop;
					break;
				}
				else
				{
					if (i == 0)
					{
						if (RayNumber == NumberOfRays)
							//goto Label_EndStageLoop;
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
				int asdg = 0;
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
					CopyVec3_refactored(IncomingRays[PreviousStageDataArrayIndex].Pos, PosRayGlob);
					CopyVec3_refactored(IncomingRays[PreviousStageDataArrayIndex].Cos, CosRayGlob);
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
					CopyVec3_refactored(IncomingRays[PreviousStageDataArrayIndex].Pos, PosRayGlob);
					CopyVec3_refactored(IncomingRays[PreviousStageDataArrayIndex].Cos, CosRayGlob);
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
					TRayData::ray_t* p_ray = Stage->RayData.Append(LastPosRaySurfStage,
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
						if (i == 0) RayNumber++; // generate new sun ray
						
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
			continue;	// No rays to carry forward
		}

		if (PreviousStageDataArrayIndex < IncomingRays.size())
		{
			LastRayNumberInPreviousStage = IncomingRays[PreviousStageDataArrayIndex].Num;
			if (LastRayNumberInPreviousStage == 0)
			{
				size_t pp = IncomingRays[PreviousStageDataArrayIndex - 1].Num;
				System->errlog("LastRayNumberInPreviousStage=0, stage %d, PrevIdx=%d, CurIdx=%d, pp=%d", i + 1,
					PreviousStageDataArrayIndex, StageDataArrayIndex, pp);
				return false;
			}
		}
		else
		{
			System->errlog("Invalid PreviousStageDataArrayIndex: %u, @ stage %d",
				PreviousStageDataArrayIndex, i + 1);
			return false;
		}

	}

}

bool Trace_refactored(TSystem *System, unsigned int seed,
		   st_uint_t NumberOfRays, 
		   st_uint_t MaxNumberOfRays,
		   bool IncludeSunShape, 
		   bool IncludeErrors,
           bool AsPowerTower,
		   int (*callback)(st_uint_t ntracedtotal, st_uint_t ntraced, st_uint_t ntotrace, st_uint_t curstage, st_uint_t nstages, void *data),
		   void *cbdata,
           std::vector< std::vector< double > > *st0data,
           std::vector< std::vector< double > > *st1in,
           bool save_st_data)
{
	return Trace_refactored_scratch(System, seed,
		NumberOfRays, MaxNumberOfRays, IncludeSunShape,
		IncludeErrors,
		AsPowerTower, callback,
		cbdata);
}




