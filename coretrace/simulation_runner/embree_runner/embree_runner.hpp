#ifndef SOLTRACE_EMBREE_RUNNER_H
#define SOLTRACE_EMBREE_RUNNER_H

#include <embree4/rtcore.h>

#include <simulation_data.hpp>
#include <simulation_runner.hpp>

#include <native_runner.hpp>
#include <native_runner_types.hpp>

namespace SolTrace::EmbreeRunner
{
    using SolTrace::Runner::RunnerStatus;
    using SolTrace::Runner::SimulationRunner;

    // NativeRunner types that are used here that we want to make visible
    // through the EmbreeRunner namespace
    using SolTrace::NativeRunner::TRayData;
    using SolTrace::NativeRunner::TSystem;
    using SolTrace::NativeRunner::tstage_ptr;
    using SolTrace::NativeRunner::telement_ptr;

    class EmbreeRunner : public SolTrace::NativeRunner::NativeRunner
    {
    public:
        EmbreeRunner();
        virtual ~EmbreeRunner();

        EmbreeRunner(const EmbreeRunner &) = delete;
        EmbreeRunner(EmbreeRunner &&) = delete;

        virtual RunnerStatus setup_simulation(const SolTrace::Data::SimulationData *data);
        virtual RunnerStatus run_simulation();
        virtual RunnerStatus update_simulation(const SolTrace::Data::SimulationData *data);

        // TODO: Do we want loud errors when a user calls these?
        void disable_power_tower() = delete;
        void enable_power_tower() = delete;
        void disable_point_focus() = delete;
        void enable_point_focus() = delete;

    private:
        // // Number of threads to use when tracing
        // uint_fast64_t number_of_threads;
        // std::vector<unsigned int> seeds;

        // SolTrace::NativeRunner::ElementParameters eparams;
        // SolTrace::NativeRunner::TSystem tsys;

        // bool set_aperture_planes(SolTrace::NativeRunner::TSystem *tsys);
        // bool set_aperture_planes(SolTrace::NativeRunner::tstage_ptr stage);
        // bool aperture_plane(SolTrace::NativeRunner::telement_ptr Element);

        RTCDevice embree_device;
        RTCScene embree_scene;

        void clean_embree();
    };

} // namespace SolTrace::EmbreeRunner

#endif
