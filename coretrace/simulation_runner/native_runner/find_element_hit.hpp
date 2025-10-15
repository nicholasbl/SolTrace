#ifndef SOLTRACE_FIND_ELEMENT_HIT_H
#define SOLTRACE_FIND_ELEMENT_HIT_H

#include "native_runner_types.hpp"

namespace SolTrace::NativeRunner {

void FindElementHit(
    // stage info
    const int i,
    const tstage_ptr Stage,
    const bool PT_override,
    const bool AsPowerTower,
    // element info
    const int nintelements,
    const std::vector<void *> &sunint_elements,
    const std::vector<void *> &reflint_elements,
    // ray info
    const int RayNumber,
    const bool in_multi_hit_loop,
    double (&PosRayStage)[3],
    double (&CosRayStage)[3],
    // outputs
    double (&LastPosRaySurfElement)[3],
    double (&LastCosRaySurfElement)[3],
    double (&LastDFXYZ)[3],
    uint_fast64_t &LastElementNumber,
    uint_fast64_t &LastRayNumber,
    double (&LastPosRaySurfStage)[3],
    double (&LastCosRaySurfStage)[3],
    int &ErrorFlag,
    int &LastHitBackSide,
    bool &StageHit);

} // namespace SolTrace::NativeRunner

#endif
