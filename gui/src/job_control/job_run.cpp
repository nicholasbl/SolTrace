#include "job_run.h"

#include "analysis/ray_volume_raster.h"
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
#include <QTimer>
#include <QtGlobal>
#include <QtConcurrentRun>


// Native only for the moment

// =============================================================================

RunningJob::RunningJob(SimDataPtr data,
                       RunType    type,
                       uint32_t   thread_count,
                       ThreadRunnerBackend backend,
                       QObject*   parent)
    : QObject(parent) {

    void (*f_ptr)(QPromise<SimResult>&      promise,
                  SimDataPtr                data,
                  ThreadRunnerConfig const& config);

    switch (type) {
    case RunType::Thread: f_ptr = execute_thread_runner; break;
    case RunType::Process: f_ptr = execute_process_runner; break;
    }

#ifdef Q_OS_WASM
    f_ptr = execute_thread_runner;
#endif

    // TEMPORARY HACK WHILE WE FIX PROCESS STUFF
    f_ptr = execute_thread_runner;

    auto config = ThreadRunnerConfig { .thread_count = thread_count,
                                       .backend      = backend };

#if defined(Q_OS_WASM) && !defined(__EMSCRIPTEN_PTHREADS__)
    m_watcher = nullptr;

    QTimer::singleShot(0, this, [this, f_ptr, data, config]() {
        QPromise<SimResult> promise;
        promise.start();
        promise.setProgressRange(0, 100);
        f_ptr(promise, data, config);
        promise.finish();

        auto future = promise.future();
        if (future.resultCount() == 0) {
            emit this->error(QStringLiteral(
                "The simulation did not return a result."));
            return;
        }

        auto res = std::move(future.result());

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

    return;
#endif

    auto future = QtConcurrent::run(f_ptr, data, config);

    auto watcher = new QFutureWatcher<SimResult>(this);

    m_watcher = watcher;

    connect(watcher, &QFutureWatcher<SimResult>::finished, this, [this]() {
        auto watcher = ((QFutureWatcher<SimResult>*)(this->m_watcher));

        if (watcher->isCanceled()) { return; }

        auto res = std::move(watcher->result());

        std::visit(
            overloaded {
                [this](ResultPtr& ptr) {
                    // there should only be one, we are the only consumer

                    this->m_result = std::move(ptr);

                    emit this->finished();
                },
                [this](QString error_text) { emit this->error(error_text); },
            },
            res);
    });

    connect(watcher,
            &QFutureWatcher<SimResult>::finished,
            this,
            &QObject::deleteLater);

    connect(watcher,
            &QFutureWatcher<SimResult>::canceled,
            this,
            &QObject::deleteLater);

    connect(watcher,
            &QFutureWatcher<SimResult>::progressValueChanged,
            this,
            &RunningJob::progress_update);
    connect(watcher,
            &QFutureWatcher<SimResult>::progressTextChanged,
            this,
            &RunningJob::progress_text_update);

    connect(watcher, &QFutureWatcher<SimResult>::canceled, this, [this]() {
        emit this->error("Cancelled");
    });

    watcher->setFuture(future);
}

RunningJob::~RunningJob() = default;

std::shared_ptr<db::SimulationResult> RunningJob::take() {
    return std::move(m_result);
}

void RunningJob::pause() {
    if (!m_watcher) { return; }
    ((QFutureWatcher<SimResult>*)m_watcher)->suspend();
}
void RunningJob::resume() {
    if (!m_watcher) { return; }
    ((QFutureWatcher<SimResult>*)m_watcher)->resume();
}
void RunningJob::cancel() {
    if (!m_watcher) { return; }
    ((QFutureWatcher<SimResult>*)m_watcher)->cancel();
}
