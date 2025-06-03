#ifndef SOLTRACE_SIMULATION_PARAMETERS_H
#define SOLTRACE_SIMULATION_PARAMETERS_H

#include <cstdint>

#include "datetime.hpp"

struct SimulationParameters
{
public:
    // TODO: Figure out how to store time...
    DateTime sim_dt;

    bool include_sun_shape_errors;
    bool include_optical_errors;

    std::uint_fast64_t number_of_rays;
    std::uint_fast64_t max_number_of_rays;
    double tolerance;

    double latitude;
    double longitude;

    int seed;

    SimulationParameters() : number_of_rays(10000),
                             max_number_of_rays(1000000),
                             tolerance(0.0),
                             latitude(0.0),
                             longitude(0.0),
                             seed(0)
    {
    }
    ~SimulationParameters() {}
};

// TODO: Implement the output stream operator.

#endif
