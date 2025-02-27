#ifndef SOLTRACE_SIMULATION_RUNNER_H
#define SOLTRACE_SIMULATION_RUNNER_H

#include "simulation_data.hpp"
#include "simulation_result.hpp"

class SimulationRunner
{
public:

    SimulationRunner();
    virtual ~SimulationRunner();

    // Disable copy constructor
    SimulationRunner(const SimulationRunner &) = delete;
    // Disable move constructor
    SimulationRunner(SimulationRunner &&) = delete;
    // Disable assignment operators
    SimulationRunner& operator=(const SimulationRunner&) = delete;
    SimulationRunner& operator=(SimulationRunner&&) = delete;

    virtual int initialize() = 0;
    virtual int setup_simulation(const SimulationData *data) = 0;
    virtual int update_simulation(const SimulationData *data) = 0;
    virtual int run_simluation() = 0;
    virtual int report_simulation(SimulationResult *result) = 0;

private:

};

#endif
