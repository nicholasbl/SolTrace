#ifndef SOLTRACE_TRACE_EMBREE_H
#define SOLTRACE_TRACE_EMBREE_H

#include <simulation_runner.hpp>
#include <native_runner_types.hpp>
#include <cstdint>

namespace SolTrace::EmbreeRunner
{

    SolTrace::Runner::RunnerStatus trace_embree(
        SolTrace::NativeRunner::TSystem *System,
        unsigned int seed,
        uint_fast64_t NumberOfRays,
        uint_fast64_t MaxNumberOfRays,
        bool IncludeSunShape,
        bool IncludeErrors,
        void *embree_scene_shared);

} // namespace SolTrace::EmbreeRunner

#endif
