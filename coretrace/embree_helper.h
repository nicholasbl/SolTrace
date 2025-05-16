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

#ifndef __EMBREE_HELPER_H
#define __EMBREE_HELPER_H

#include <embree4/rtcore.h>
#include "types.h"


namespace embree_helper 
{

	struct RayIntersectPayload
	{
		RTCRayQueryContext context;	// Embree built-in context (MUST come first)

		int LastHitBackSide = -1;
		double LastDFXYZ[3] = { 0.0, 0.0, 0.0 };

		//double LastPosRaySurfGlob[3] = { 0.0, 0.0, 0.0 };
		//double LastCosRaySurfGlob[3] = { 0.0, 0.0, 0.0 };

		double LastPosRaySurfStage[3] = { 0.0, 0.0, 0.0 };
		double LastCosRaySurfStage[3] = { 0.0, 0.0, 0.0 };

		double LastPosRaySurfElement[3] = { 0.0, 0.0, 0.0 };
		double LastCosRaySurfElement[3] = { 0.0, 0.0, 0.0 };

		int element_number = -1;
		int ErrorFlag = 0;
	};

	void error_function(void* userPtr, RTCError error, const char* str);

	RTCScene make_scene(RTCDevice& device, TSystem& system);

	bool validate_intersect(double(&LastPosRaySurfElement1)[3], double(&LastCosRaySurfElement1)[3], double(&LastDFXYZ1)[3],
		st_uint_t& LastElementNumber1, st_uint_t& LastRayNumber1,
		double(&LastPosRaySurfStage1)[3], double(&LastCosRaySurfStage1)[3],
		int& ErrorFlag1, int& LastHitBackSide1,
		bool& StageHit1,

		double(&LastPosRaySurfElement2)[3], double(&LastCosRaySurfElement2)[3], double(&LastDFXYZ2)[3],
		st_uint_t& LastElementNumber2, st_uint_t& LastRayNumber2,
		double(&LastPosRaySurfStage2)[3], double(&LastCosRaySurfStage2)[3],
		int& ErrorFlag2, int& LastHitBackSide2,
		bool& StageHit2);

}	// namespace embree_helper

#endif