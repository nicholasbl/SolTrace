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

template<typename EnumT>
EnumT get_enum_from_string(const std::string& str, 
    const std::map<EnumT, std::string>& forward_map, 
    EnumT unknown)
{
    for (const auto& key_pair : forward_map)
    {
        if (key_pair.second == str)
        {
            return key_pair.first;
        }
    }

    return unknown;
}
	
bool load_stinput_file(SimulationData& sd, std::string filename);

void write_json_file(SimulationData& sd, std::string filename);

void load_json_file(SimulationData& sd, std::string filename);

} // namespace SolTrace::Data

#endif
