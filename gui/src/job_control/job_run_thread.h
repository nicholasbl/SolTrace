#pragma once

#include <QPromise>

#include "job_run_common.h"

void execute_thread_runner(QPromise<SimResult>& promise, SimDataPtr data);
