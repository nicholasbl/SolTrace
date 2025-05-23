#ifndef SOLTRACE_SUN_TO_PRIMARY_STAGE_H
#define SOLTRACE_SUN_TO_PRIMARY_STAGE_H

#include "native_runner_types.hpp"

bool SunToPrimaryStage(TSystem *System,
                       TStage *Stage,
                       TSun *Sun,
                       double PosSunStage[3]);

#endif
