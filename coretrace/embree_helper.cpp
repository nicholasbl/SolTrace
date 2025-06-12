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

#include "embree_helper.h"
#include <stdio.h>
#include "procs.h"
#include <cctype>
#include <algorithm>
#include "bbox_calculator.h"

namespace embree_helper 
{

	inline void CopyVec3(double dest[3], double src[3])
	{
		dest[0] = src[0];
		dest[1] = src[1];
		dest[2] = src[2];
	}

	template<typename T>
	bool compare_Vec3(T vec1[3], T vec2[3], double tol_diff)
	{
		for (int i = 0; i < 3; i++)
		{
			if ((std::abs(vec1[i] / vec2[i] - 1) > tol_diff)
				&& (std::abs(vec1[i] - vec2[i]) > 1e-5))
				return false;
		}
		return true;
	}

	void error_function(void* userPtr, RTCError error, const char* str)
	{
		printf("error %d: %s\n", error, str);
	}

	int bounds_error(const RTCBoundsFunctionArguments* args, const bbox_calculator::BBOXERRORS error_enum)
	{
		char* str = "";
		switch (error_enum)
		{
			case bbox_calculator::NONE:
				str = "None";
				break;
			case bbox_calculator::BOUNDS_APERTURE_ERROR:
				str = "Aperture bounds error";
				break;
			case bbox_calculator::BOUNDS_SURFACE_ERROR:
				str = "Surface bounds error";
				break;
			default:
				str = "Unknown error";
				break;
		}

		// Set bounds to zero and report error
		RTCBounds* bounds_o = args->bounds_o;
		bounds_o->lower_x = std::numeric_limits<float>::quiet_NaN();
		bounds_o->upper_x = std::numeric_limits<float>::quiet_NaN();
		bounds_o->lower_y = std::numeric_limits<float>::quiet_NaN();
		bounds_o->upper_y = std::numeric_limits<float>::quiet_NaN();
		bounds_o->lower_z = std::numeric_limits<float>::quiet_NaN();
		bounds_o->upper_z = std::numeric_limits<float>::quiet_NaN();

		error_function(args->geometryUserPtr, RTC_ERROR_INVALID_OPERATION,
			str);

		throw std::runtime_error("Embree scene commit failed");
	}

	void bounds_function(const RTCBoundsFunctionArguments* args)
	{
		// Get element
		TElement* st_element = (TElement*)args->geometryUserPtr;
		
		// Get bounds
		float min_coord_global[3];
		float max_coord_global[3];
		bbox_calculator::BBOXERRORS error = bbox_calculator::get_bounds(st_element, min_coord_global, max_coord_global);
		
		// Check error
		if (error != bbox_calculator::NONE)
		{
			bounds_error(args, error);
		}

		// Assign bounds
		RTCBounds* bounds_o = args->bounds_o;
		bounds_o->lower_x = min_coord_global[0];
		bounds_o->upper_x = max_coord_global[0];
		bounds_o->lower_y = min_coord_global[1];
		bounds_o->upper_y = max_coord_global[1];
		bounds_o->lower_z = min_coord_global[2];
		bounds_o->upper_z = max_coord_global[2];
	}

	void intersect_function(const RTCIntersectFunctionNArguments* args)
	{
		// Retrieve ray data (GLOBAL coordinates, may NOT be normalized/unit)
		//const RTCRayHit* rayhit = (const RTCRayHit*)args->rayhit;
		//double PosRayGlob[3] = { rayhit->ray.org_x, rayhit->ray.org_y, rayhit->ray.org_z };
		//double CosRayGlob[3] = { rayhit->ray.dir_x, rayhit->ray.dir_y, rayhit->ray.dir_z };	// ASSUMING these are normalized (dangerous)

		// Get payload object
		RayIntersectPayload* payload = (RayIntersectPayload*)args->context;

		double PosRayGlob[3], CosRayGlob[3];
		CopyVec3(PosRayGlob, payload->PosRayGlobIn);
		CopyVec3(CosRayGlob, payload->CosRayGlobIn);

		// Get Element data
		TElement* st_element = (TElement*)args->geometryUserPtr;
		TStage* st_stage = (TStage*)st_element->parent_stage;
		

		// First, convert ray coordinates to element
		// Global -> stage -> element
		// transform the global incoming ray to local stage coordinates
		double PosRayStage[3], CosRayStage[3];
		::TransformToLocal(PosRayGlob, CosRayGlob,
			st_stage->Origin, st_stage->RRefToLoc,
			PosRayStage, CosRayStage);

		//  {Transform ray to element[j] coord system of Stage[i]}
		double PosRayElement[3], CosRayElement[3];
		::TransformToLocal(PosRayStage, CosRayStage,
			st_element->Origin, st_element->RRefToLoc,
			PosRayElement, CosRayElement);

		// increment position by tiny amount to get off the element if tracing to the same element
		PosRayElement[0] = PosRayElement[0] + 1.0e-4 * CosRayElement[0];
		PosRayElement[1] = PosRayElement[1] + 1.0e-4 * CosRayElement[1];
		PosRayElement[2] = PosRayElement[2] + 1.0e-4 * CosRayElement[2];

		// Call DeterminElementIntersectionNew
		double PosRaySurfElement[3] = { 0.0, 0.0, 0.0 };
		double CosRaySurfElement[3] = { 0.0, 0.0, 0.0 };
		double DFXYZ[3] = { 0.0, 0.0, 0.0 };
		double PathLength = 0;

		int InterceptFlag = 0;
		int HitBackSide = 0;
		::DetermineElementIntersectionNew(st_element, PosRayElement, CosRayElement,
			PosRaySurfElement, CosRaySurfElement, DFXYZ,
			&PathLength, &payload->ErrorFlag, &InterceptFlag, &HitBackSide);

		// Update rayhit info (if hit)
		if (InterceptFlag != 0)
		{
			// Get rayhit data
			RTCRayHit* rayhit_out = (RTCRayHit*)args->rayhit;

			// Check if hit is closer than other hits
			// Using payload pathlength for double precision
			// If pathlength == lastpathlength, use which element number is lower
			// to match results with original code
			if ((PathLength < payload->LastPathLength) 
				|| ((PathLength == payload->LastPathLength) 
					&& (st_element->element_number < payload->element_number)))
			{

				if (PosRaySurfElement[2] <= st_element->ZAperture
					|| st_element->SurfaceIndex == 'm'
					|| st_element->SurfaceIndex == 'M'
					|| st_element->SurfaceIndex == 'r'
					|| st_element->SurfaceIndex == 'R')
				{

					// Transform ray back to stage coordinate system
					double PosRaySurfStage[3] = { 0.0, 0.0, 0.0 };
					double CosRaySurfStage[3] = { 0.0, 0.0, 0.0 };
					::TransformToReference(PosRaySurfElement, CosRaySurfElement,
						st_element->Origin, st_element->RLocToRef,
						PosRaySurfStage, CosRaySurfStage);

					rayhit_out->ray.tfar = (float)PathLength; // Update intersection distance
					rayhit_out->hit.geomID = args->geomID;
					rayhit_out->hit.primID = 0; // Single primitive

					// Define Ng (will not be used)
					rayhit_out->hit.Ng_x = std::numeric_limits<float>::quiet_NaN();
					rayhit_out->hit.Ng_y = std::numeric_limits<float>::quiet_NaN();
					rayhit_out->hit.Ng_z = std::numeric_limits<float>::quiet_NaN();

					// Assign custom outputs
					payload->LastHitBackSide = HitBackSide;
					CopyVec3(payload->LastDFXYZ, DFXYZ);
					//CopyVec3(payload->LastPosRaySurfGlob, PosRaySurfGlob);
					//CopyVec3(payload->LastCosRaySurfGlob, CosRaySurfGlob);
					CopyVec3(payload->LastPosRaySurfStage, PosRaySurfStage);
					CopyVec3(payload->LastCosRaySurfStage, CosRaySurfStage);
					CopyVec3(payload->LastPosRaySurfElement, PosRaySurfElement);
					CopyVec3(payload->LastCosRaySurfElement, CosRaySurfElement);
					payload->element_number = st_element->element_number;
					payload->LastPathLength = PathLength;

				}
			}
			
		}
		else
		{
			return;
		}
	}

	RTCScene make_scene(RTCDevice& device, TSystem& system)
	{
		// Make scene
		RTCScene scene = rtcNewScene(device);

		// Loop through stages
		unsigned int stage_index = 1;
		for (TStage* stage : system.StageList)
		{
			// Loop through elements in each stage
			int j = 0;
			unsigned int stage_mask = 1u << stage_index;
			for (TElement* st_element : stage->ElementList)
			{
				// Skip if not enabled
				if (st_element->Enabled == false)
				{
					j++;
					continue;
				}

				// Add ptr to parent stage to st_element
				st_element->parent_stage = stage;
				st_element->element_number = j + 1;
				st_element->embree_mask = stage_mask;

				// Make embree geometry for each element
				RTCGeometry embree_geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_USER);
				rtcSetGeometryUserPrimitiveCount(embree_geom, 1);

				// Attach st element to embree geometry
				rtcSetGeometryUserData(embree_geom, st_element);

				// Assign bounds function
				rtcSetGeometryBoundsFunction(embree_geom, bounds_function, NULL);

				// Assign intersect function
				rtcSetGeometryIntersectFunction(embree_geom, intersect_function);

				// Set mask (stage number)
				rtcSetGeometryMask(embree_geom, stage_mask);

				// Commit geometry
				rtcCommitGeometry(embree_geom);

				// Attach geometry
				unsigned int geomID = rtcAttachGeometry(scene, embree_geom);

				// Release geometry (it is owned by the scene now)
				rtcReleaseGeometry(embree_geom);

				j++;
			}

			stage_index++;
		}

		return scene;
	}

	bool validate_intersect(double(&LastPosRaySurfElement1)[3], double(&LastCosRaySurfElement1)[3], double(&LastDFXYZ1)[3],
		st_uint_t& LastElementNumber1, st_uint_t& LastRayNumber1,
		double(&LastPosRaySurfStage1)[3], double(&LastCosRaySurfStage1)[3],
		int& ErrorFlag1, int& LastHitBackSide1,
		bool& StageHit1,

		double(&LastPosRaySurfElement2)[3], double(&LastCosRaySurfElement2)[3], double(&LastDFXYZ2)[3],
		st_uint_t& LastElementNumber2, st_uint_t& LastRayNumber2,
		double(&LastPosRaySurfStage2)[3], double(&LastCosRaySurfStage2)[3],
		int& ErrorFlag2, int& LastHitBackSide2,
		bool& StageHit2)
	{
		if (StageHit1 == false && StageHit2 == false)
			return true;

		double tol_diff = 1e-4;
		if (StageHit1 != StageHit2) return false;
		if (LastElementNumber1 != LastElementNumber2) return false;
		if (LastHitBackSide1 != LastHitBackSide2) return false;
		if (!compare_Vec3(LastPosRaySurfElement1, LastPosRaySurfElement2, tol_diff)) return false;
		if (!compare_Vec3(LastCosRaySurfElement1, LastCosRaySurfElement2, tol_diff)) return false;
		if (!compare_Vec3(LastDFXYZ1, LastDFXYZ2, tol_diff)) return false;
		if (!compare_Vec3(LastPosRaySurfStage1, LastPosRaySurfStage2, tol_diff)) return false;
		if (!compare_Vec3(LastCosRaySurfStage1, LastCosRaySurfStage2, tol_diff)) return false;

		return true;
	}

}	// namespace embree_helper