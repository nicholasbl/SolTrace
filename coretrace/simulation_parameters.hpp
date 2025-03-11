#ifndef SOLTRACE_SIMULATION_PARAMETERS_H
#define SOLTRACE_SIMULATION_PARAMETERS_H

#include "datetime.hpp"

class SimulationParameters
{

public:
    SimulationParameters();
    ~SimulationParameters();

private:
    // TODO: Figure out how to store time...
    DateTime sim_dt;

    unsigned long number_of_rays;
    double tolerance;

    double latitude;
    double longitude;

    int seed;
};

#endif
