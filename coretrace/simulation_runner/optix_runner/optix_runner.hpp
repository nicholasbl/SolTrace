#ifndef SOLTRACE_OPTIX_RUNNER_H
#define SOLTRACE_OPTIX_RUNNER_H

#include "simulation_result.hpp"
#include "simulation_runner/simulation_runner.hpp"
#include "core/soltrace_system.h"

class OptixRunner : public SimulationRunner
{
public:
    OptixRunner();
    ~OptixRunner();

    virtual RunnerStatus initialize();
    virtual RunnerStatus setup_simulation(const SimulationData *data);
    virtual RunnerStatus update_simulation(const SimulationData *data);
    virtual RunnerStatus run_simulation();
    virtual RunnerStatus report_simulation(SimulationResult *result,
                                           int level_spec);

    // Runner options
    // void disable_sun_shape_errors() { this->include_sun_shape_errors = false; }
    // void enable_sun_shape_errors() { this->include_sun_shape_errors = true; }
    // void disable_errors() { this->include_errors = false; }
    // void enable_errors() { this->include_errors = true; }


    // Runner accessors
    //const TSystem *get_system() const { return &this->tsys; }

private:

    SolTraceSystem m_sys;

    const SimulationData *m_simdata;  
    RunnerStatus setup_parameters(const SimulationData *data);
    RunnerStatus setup_sun(const SimulationData *data);
    RunnerStatus setup_elements(const SimulationData *data);
};

#endif
