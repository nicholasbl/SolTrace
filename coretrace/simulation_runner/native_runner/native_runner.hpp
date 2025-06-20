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
    void disable_power_tower() { this->as_power_tower = false; }
    void enable_power_tower() { this->as_power_tower = true; }
    void disable_point_focus() { this->tsys.sim_dynamic_group = false; }
    void enable_point_focus() { this->tsys.sim_dynamic_group = true; }
    void set_newton_tolerance(double tol)
    {
        this->eparams.newton_tolerance = tol;
        return;
    }

    void set_newton_max_iters(uint_fast64_t max_iters)
    {
        this->eparams.newton_max_iters = max_iters;
        return;
    }

    void set_number_of_threads(uint_fast64_t nthr)
    {
        this->number_of_threads = nthr;
        return;
    }

    const TSystem *get_system() const { return &this->tsys; }

    RunnerStatus setup_parameters(const SimulationData *data);
    RunnerStatus setup_sun(const SimulationData *data);
    RunnerStatus setup_elements(const SimulationData *data);

private:
    // Use power tower speed ups
    bool as_power_tower;

    // Number of threads to use when tracing
    uint_fast64_t number_of_threads;

    ElementParameters eparams;
    
    TSystem tsys;
};

#endif
