#ifndef SOLTRACE_TRACE_EMBREE_H
#define SOLTRACE_TRACE_EMBREE_H

#include <cstdint>

#include <embree4/rtcore.h>
#include <native_runner_types.hpp>
#include <simulation_runner.hpp>
#include <trace_logger.hpp>

namespace SolTrace::EmbreeRunner
{

    SolTrace::Runner::RunnerStatus make_embree_scene(
        SolTrace::NativeRunner::trace_logger_ptr logger,
        SolTrace::NativeRunner::TSystem *System,
        RTCDevice &embree_device,
        RTCScene &embree_scene);

    SolTrace::Runner::RunnerStatus trace_embree(
        SolTrace::NativeRunner::trace_logger_ptr logger,
        SolTrace::NativeRunner::TSystem *System,
        unsigned int seed,
        uint_fast64_t NumberOfRays,
        uint_fast64_t MaxNumberOfRays,
        bool IncludeSunShape,
        bool IncludeErrors,
        const RTCScene &embree_scene);

} // namespace SolTrace::EmbreeRunner

#endif
