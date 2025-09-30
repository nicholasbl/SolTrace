
#ifndef SIMDATA_BRIDGE_H
#define SIMDATA_BRIDGE_H

#include "simulation_data/simulation_data.hpp"
#include "types.h"

enum class ConversionErrors
{
	SUCCESS = 0,
	NOT_SUPPORTED_ERROR,
	APERTURE_ERROR,
	SURFACE_ERROR,
	ELEMENT_ADD_ERROR,
	STAGE_ADD_ERROR
};

int convert_tsystem_to_sim_data(TSystem* sys, SolTrace::Data::SimulationData& sd);

int run_native_runner(SolTrace::Data::SimulationData& sd, TSystem* sys);

int run_optix_runner(SolTrace::Data::SimulationData& sd, TSystem* sys);

int run_optix_file_runner(TSystem* sys, const char* file_name);

#endif