#ifndef SOLTRACE_TRACE_EMBREE_H
#define SOLTRACE_TRACE_EMBREE_H

#include <cstdint>

#include <native_runner_types.hpp>
#include <simulation_runner.hpp>
#include <trace_logger.hpp>

namespace SolTrace::EmbreeRunner
{

    SolTrace::Runner::RunnerStatus trace_embree(
        SolTrace::NativeRunner::trace_logger_ptr logger,
        SolTrace::NativeRunner::TSystem *System,
        unsigned int seed,
        uint_fast64_t NumberOfRays,
        uint_fast64_t MaxNumberOfRays,
        bool IncludeSunShape,
        bool IncludeErrors,
        void *embree_scene_shared);

} // namespace SolTrace::EmbreeRunner

#endif
