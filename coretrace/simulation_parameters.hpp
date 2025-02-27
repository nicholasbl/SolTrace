#ifndef SOLTRACE_SIMULATION_PARAMETERS_H
#define SOLTRACE_SIMULATION_PARAMETERS_H

class SimulationParameters
{

public:
    SimulationParameters();
    ~SimulationParameters();

private:
    // TODO: Figure out how to store time...

    unsigned long number_of_rays;
    double tolerance;

    double latitude;
    double longitude;

    int seed;
};

#endif
