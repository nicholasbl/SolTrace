#ifndef SOLTRACE_SUN_TO_PRIMARY_STAGE_H
#define SOLTRACE_SUN_TO_PRIMARY_STAGE_H

#include "native_runner_types.hpp"

namespace SolTrace::NativeRunner {

bool SunToPrimaryStage(TSystem *System,
                       TStage *Stage,
                       TSun *Sun,
                       double PosSunStage[3]);

} // namespace SolTrace::NativeRunner

#endif
