#ifndef SOLTRACE_NATIVE_RUNNER_H
#define SOLTRACE_NATIVE_RUNNER_H

#include "native_runner_types.hpp"
#include "simulation_result.hpp"
#include "simulation_runner.hpp"

class NativeRunner : public SimulationRunner
{
public:
    NativeRunner();
    ~NativeRunner();

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
    void disable_power_tower() { this->as_power_tower = false; }
    void enable_power_tower() { this->as_power_tower = true; }

    // Runner accessors
    const TSystem *get_system() const { return &this->tsys; }

private:
    // bool include_sun_shape_errors;
    // bool include_errors;
    bool as_power_tower;
    
    // // SimulationData to use for ray tracing computations
    // const SimulationData *simdata;
    TSystem tsys;

    RunnerStatus setup_parameters(const SimulationData *data);
    RunnerStatus setup_sun(const SimulationData *data);
    RunnerStatus setup_elements(const SimulationData *data);
};

#endif
