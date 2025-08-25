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

bool load_stinput_file(SimulationData& sd, std::string filename);

#endif
