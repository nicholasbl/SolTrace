#ifndef SOLTRACE_OPTIX_RUNNER_H
#define SOLTRACE_OPTIX_RUNNER_H

#include "simulation_data.hpp"
#include "simulation_result.hpp"
#include "simulation_runner.hpp"
#include "core/soltrace_system.h"

// using SolTrace::Runner::RunnerStatus;

class OptixRunner : public SolTrace::Runner::SimulationRunner
{
public:
    OptixRunner();
    ~OptixRunner();

    virtual SolTrace::Runner::RunnerStatus initialize() override;
    virtual SolTrace::Runner::RunnerStatus setup_simulation(
        const SolTrace::Data::SimulationData *data) override;
    virtual SolTrace::Runner::RunnerStatus update_simulation(
        const SolTrace::Data::SimulationData *data) override;
    virtual SolTrace::Runner::RunnerStatus run_simulation() override;
    virtual SolTrace::Runner::RunnerStatus status_simulation(double *progress = nullptr) override;
    virtual SolTrace::Runner::RunnerStatus cancel_simulation() override;
    virtual SolTrace::Runner::RunnerStatus report_simulation(
        SolTrace::Result::SimulationResult *result,
        int level_spec) override;

    SolTrace::Runner::RunnerStatus run_simulation_core();
    SolTrace::Runner::RunnerStatus get_hp_output(std::vector<float4>& hp_vec,
        std::vector<uint_fast64_t>& raynumber_vec, std::vector<int32_t>& element_id_vec);

    double get_sun_plane_area() { return m_sys.get_sun_plane_area(); }

    uint_fast64_t get_N_sun_rays() { return m_sys.get_N_sun_rays(); }

    void print_timing() const;

    void set_verbose(bool verbose);

    // Runner options
    // void disable_sun_shape_errors() { this->include_sun_shape_errors = false; }
    // void enable_sun_shape_errors() { this->include_sun_shape_errors = true; }
    // void disable_errors() { this->include_errors = false; }
    // void enable_errors() { this->include_errors = true; }

    // Runner accessors
    OptixCSP::SolTraceSystem *get_optix_system() { return &this->m_sys; }

private:
    OptixCSP::SolTraceSystem m_sys;

    const SolTrace::Data::SimulationData *m_simdata;
    SolTrace::Runner::RunnerStatus setup_parameters(
        const SolTrace::Data::SimulationData *data);
    SolTrace::Runner::RunnerStatus setup_sun(
        const SolTrace::Data::SimulationData *data);
    SolTrace::Runner::RunnerStatus setup_elements(
        const SolTrace::Data::SimulationData *data);

    // helper function, convert Vector3d to Optix::Vec3d
    OptixCSP::Vec3d ToVec3d(glm::dvec3 v);
    OptixCSP::Matrix33d ToMatrix33d(const glm::dmat3& mat);
    // helper function, convert SolTrace::Data::DistributionType to Optix::OpticalDistribution
    OptixCSP::OpticalDistribution to_optical_distribution(SolTrace::Data::DistributionType dt);
    
};

#endif
