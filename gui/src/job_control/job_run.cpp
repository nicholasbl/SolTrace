#include "job_run.h"

#include "analysis/ray_volume_raster.h"
#include "dataset.h"
#include "job_run_process.h"
#include "job_run_thread.h"
#include "native_runner/native_runner.hpp"
#include "simulation_result.hpp"
#include "simulation_runner.hpp"
#include "utilities/math_utility.h"

#include <QDir>
#include <QException>
#include <QFutureWatcher>
#include <QGuiApplication>
#include <QPromise>
#include <QtConcurrentRun>


// Native only for the moment

// =============================================================================

RunningJob::RunningJob(SimDataPtr data, RunType type, QObject* parent)
    : QObject(parent) {

    void (*f_ptr)(QPromise<SimResult>& promise, SimDataPtr data);

    switch (type) {
    case RunType::Thread: f_ptr = execute_thread_runner; break;
    case RunType::Process: f_ptr = execute_process_runner; break;
    }

#ifdef Q_WASM
    f_ptr = execute_thread_runner;
#endif

    // TEMPORARY HACK WHILE WE FIX PROCESS STUFF
    f_ptr = execute_thread_runner;

    auto future = QtConcurrent::run(f_ptr, data);

    auto watcher = new QFutureWatcher<SimResult>();

    m_watcher = watcher;

    connect(watcher, &QFutureWatcher<Result>::finished, this, [this]() {
        auto watcher = ((QFutureWatcher<SimResult>*)(this->m_watcher));

        auto res = watcher->result();

        std::visit(
            overloaded {
                [this](ResultPtr& ptr) {
                    this->m_result = std::move(ptr);

                    emit this->finished();
                },
                [this](QString error_text) { emit this->error(error_text); },
            },
            res);
    });

    connect(watcher,
            &QFutureWatcher<Result>::finished,
            this,
            &QObject::deleteLater);

    connect(watcher,
            &QFutureWatcher<Result>::canceled,
            this,
            &QObject::deleteLater);

    connect(watcher,
            &QFutureWatcher<Result>::progressValueChanged,
            this,
            &RunningJob::progress_update);
    connect(watcher,
            &QFutureWatcher<Result>::progressTextChanged,
            this,
            &RunningJob::progress_text_update);

    connect(watcher, &QFutureWatcher<Result>::canceled, this, [this]() {
        emit this->error("Cancelled");
    });

    watcher->setFuture(future);
}

RunningJob::~RunningJob() = default;

std::shared_ptr<ResultDB> RunningJob::take() {
    return std::move(m_result);
}

void RunningJob::pause() {
    ((QFutureWatcher<SimResult>*)m_watcher)->suspend();
}
void RunningJob::resume() {
    ((QFutureWatcher<SimResult>*)m_watcher)->resume();
}
void RunningJob::cancel() {
    ((QFutureWatcher<SimResult>*)m_watcher)->cancel();
}
