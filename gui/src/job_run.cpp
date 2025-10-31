#include "job_run.h"

#include "native_runner/native_runner.hpp"
#include "simulation_result.hpp"
#include "simulation_runner.hpp"
#include "utility.h"

#include <QException>
#include <QFutureWatcher>
#include <QGuiApplication>
#include <QPromise>
#include <QtConcurrentRun>

// Native only for the moment

using ResultPtr = std::unique_ptr<SolTrace::Result::SimulationResult>;
using SimResult = std::variant<ResultPtr, QString>;

// Qconcurrent will auto call start and finish on the promise

#define SECTION(FUNC, TEXT)                                                    \
    promise.setProgressValueAndText(0, TEXT);                                  \
    promise.suspendIfRequested();                                              \
    if (promise.isCanceled()) { return; }                                      \
    result = current_runner->FUNC;                                             \
    if (result == SolTrace::Runner::RunnerStatus::ERROR) {                     \
        promise.emplaceResult(QString(TEXT " failed"));                        \
        return;                                                                \
    }

static void execute_runner(QPromise<SimResult>& promise, SimDataPtr data) {
    try {
        promise.setProgressRange(0, 100);

        auto current_runner =
            std::make_unique<SolTrace::NativeRunner::NativeRunner>();

        SolTrace::Runner::RunnerStatus result;

        SECTION(initialize(), "Starting simulation");

        SECTION(setup_simulation(data.get()), "Setup simulation");

        SECTION(run_simulation(), "Run simulation");

        auto ret = std::make_unique<SolTrace::Result::SimulationResult>();

        SECTION(report_simulation(ret.get(), 100), "Report simulation");

        promise.emplaceResult(std::move(ret));

    } catch (std::exception& e) {
        promise.emplaceResult(QString(e.what()));
        return;
    }
}

void RunningJob::setup_thread(SimDataPtr data) {
    auto future = QtConcurrent::run(execute_runner, data);

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
void RunningJob::setup_process(SimDataPtr data) {
#ifdef Q_OS_WASM
    return setup_thread(data);
#else
    {
        // dump to a temp file or memory

        // launch another process using this
        // qApp->applicationFilePath();

        // have it read the stuff and return

        return setup_thread(data);
    }
#endif
}

RunningJob::RunningJob(SimDataPtr data, RunType type, QObject* parent)
    : QObject(parent) {
    switch (type) {
    case RunType::Thread: setup_thread(data); break;
    case RunType::Process: setup_process(data); break;
    }
}

RunningJob::~RunningJob() = default;

std::unique_ptr<RunningJob::Result> RunningJob::take() {
    return std::move(m_result);
}

void RunningJob::pause() {
    ((QFutureWatcher<SimResult>*)m_watcher)->suspend();
}
void RunningJob::resume() {
    ((QFutureWatcher<SimResult>*)m_watcher)->resume();
}
