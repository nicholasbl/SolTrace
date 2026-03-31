#pragma once

#include <QPromise>

#include "job_run_common.h"

struct ThreadRunnerConfig {
    uint32_t thread_count;
};

void execute_thread_runner(QPromise<SimResult>&      promise,
                           SimDataPtr                data,
                           ThreadRunnerConfig const& config);
