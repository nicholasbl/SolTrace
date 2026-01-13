#ifndef SOLTRACE_EMBREE_RUNNER_H
#define SOLTRACE_EMBREE_RUNNER_H

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

        // ---- Use inherited methods for these ---- //
        // -- TODO: Remove this comment block once things have stabilized -- //
        // virtual RunnerStatus initialize();
        // virtual RunnerStatus update_simulation(const SolTrace::Data::SimulationData *data);
        // virtual RunnerStatus report_simulation(SolTrace::Result::SimulationResult *result,
        //                                        int level_spec);
        // void set_newton_tolerance(double tol)
        // {
        //     this->eparams.newton_tolerance = tol;
        //     return;
        // }
        // void set_newton_max_iters(uint_fast64_t max_iters)
        // {
        //     this->eparams.newton_max_iters = max_iters;
        //     return;
        // }
        // void print_log(std::ostream &os)
        // {
        //     // this->my_manager->print_log(os);
        //     return;
        // }
        // // Accessors
        // int_fast64_t get_number_stages() const
        // {
        //     return this->get_system()->StageList.size();
        // }
        // int_fast64_t get_number_elements() const
        // {
        //     int_fast64_t nelems = 0;
        //     for (auto stage : this->get_system()->StageList)
        //     {
        //         nelems += stage->ElementList.size();
        //     }
        //     return nelems;
        // }
        // const SolTrace::NativeRunner::TSystem *get_system() const
        // {
        //     return &this->tsys;
        // }
        // // Helper functions
        // RunnerStatus setup_parameters(const SolTrace::Data::SimulationData *data);
        // RunnerStatus setup_sun(const SolTrace::Data::SimulationData *data);
        // RunnerStatus setup_elements(const SolTrace::Data::SimulationData *data);
        // ---- End Inherited Functions Notes ---- //

        virtual RunnerStatus setup_simulation(const SolTrace::Data::SimulationData *data);
        virtual RunnerStatus run_simulation();
        virtual RunnerStatus status_simulation(double *progress = nullptr);
        virtual RunnerStatus cancel_simulation();

        void set_number_of_threads(uint_fast64_t nthr)
        {
            // this->number_of_threads = nthr;
            // this->seeds.clear();
            return;
        }

        // void set_number_of_threads(uint_fast64_t nthr,
        //                            const std::vector<unsigned int> &seeds)
        // {
        //     if (nthr == seeds.size())
        //     {
        //         this->number_of_threads = nthr;
        //         this->seeds = seeds;
        //     }
        //     else
        //     {
        //         throw std::invalid_argument("Number of seeds must equal number of threads.");
        //     }
        //     return;
        // }

    private:
        // // Number of threads to use when tracing
        // uint_fast64_t number_of_threads;
        // std::vector<unsigned int> seeds;

        // SolTrace::NativeRunner::ElementParameters eparams;
        // SolTrace::NativeRunner::TSystem tsys;

        // bool set_aperture_planes(SolTrace::NativeRunner::TSystem *tsys);
        // bool set_aperture_planes(SolTrace::NativeRunner::tstage_ptr stage);
        // bool aperture_plane(SolTrace::NativeRunner::telement_ptr Element);
    };

} // namespace SolTrace::EmbreeRunner

#endif
