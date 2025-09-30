/**
 * @file simdata_io.hpp
 * @brief Simulation data input/output operations
 *
 * Provides functions for reading and writing simulation data
 * to various file formats and data exchange mechanisms.
 * Includes support for SolTrace input files (.stinput) and
 * other data formats.
 */

#ifndef SIMDATA_H
#define SIMDATA_H

#include <string>
#include "simulation_data.hpp"

namespace SolTrace::Data {

DistributionType char_to_distribution(const char dist_char);

ApertureType char_to_aperture(const char aperture_char);

SurfaceType char_to_surface(const char surface_char);

InteractionType int_to_interaction(const int interaction_int);

bool load_stinput_file(SimulationData& sd, std::string filename);

} // namespace SolTrace::Data

#endif
