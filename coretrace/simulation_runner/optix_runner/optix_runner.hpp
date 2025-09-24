#ifndef SOLTRACE_OPTIX_RUNNER_H
#define SOLTRACE_OPTIX_RUNNER_H
#include "simulation_result.hpp"
#include "simulation_runner.hpp"
#include "core/soltrace_system.h"

class OptixRunner : public SimulationRunner
{
public:
    OptixRunner();
    ~OptixRunner();

    virtual RunnerStatus initialize();
    virtual RunnerStatus setup_simulation(const SolTrace::Data::SimulationData *data);
    virtual RunnerStatus update_simulation(const SolTrace::Data::SimulationData *data);
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

    OptixCSP::SolTraceSystem m_sys;

    const SimulationData *m_simdata;  
    RunnerStatus setup_parameters(const SolTrace::Data::SimulationData *data);
    RunnerStatus setup_sun(const SolTrace::Data::SimulationData *data);
    RunnerStatus setup_elements(const SolTrace::Data::SimulationData *data);

    // helper function, convert Vector3d to Optix::Vec3d
    OptixCSP::Vec3d ToVec3d(SolTrace::Data::Vector3d v);
};

#endif
