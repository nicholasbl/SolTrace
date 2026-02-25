#include "job_run_thread.h"

#include "native_runner/native_runner.hpp"
#include "simulation_result.hpp"
#include "simulation_runner.hpp"
#include "utilities/math_utility.h"

#include <QtConcurrentRun>

// Qconcurrent will auto call start and finish on the promise

#define SOLTRACE_SECTION(FUNC, VALUE, TEXT)                                    \
    qDebug() << Q_FUNC_INFO << TEXT;                                           \
    promise.setProgressValueAndText(VALUE, TEXT);                              \
    promise.suspendIfRequested();                                              \
    if (promise.isCanceled()) { return; }                                      \
    result = current_runner->FUNC;                                             \
    if (result == SolTrace::Runner::RunnerStatus::ERROR) {                     \
        qWarning() << Q_FUNC_INFO << "failed at" << TEXT << (int)result;       \
        promise.emplaceResult(QString(TEXT " failed"));                        \
        return;                                                                \
    }

#define SECTION(VALUE, TEXT)                                                   \
    qDebug() << Q_FUNC_INFO << TEXT;                                           \
    promise.setProgressValueAndText(VALUE, TEXT);                              \
    promise.suspendIfRequested();                                              \
    if (promise.isCanceled()) {                                                \
        promise.emplaceResult("Cancelled at " TEXT);                           \
        return;                                                                \
    }

/// Function to wrap up run into a thread. We will be checking progress
/// OUTSIDE this function
static SolTrace::Runner::RunnerStatus
execute_solve_with(SolTrace::Runner::SimulationRunner* ptr) {
    qDebug() << "execute runner";
    return ptr->run_simulation();
}

void execute_thread_runner(QPromise<SimResult>& promise, SimDataPtr data) {
    try {
        auto start_instant = std::chrono::high_resolution_clock::now();

        promise.setProgressRange(0, 100);

        auto current_runner =
            std::make_unique<SolTrace::NativeRunner::NativeRunner>();

        SolTrace::Runner::RunnerStatus result;

        size_t thread_count = std::thread::hardware_concurrency();
        if (thread_count == 0) { thread_count = 1; }

        current_runner->set_number_of_threads(thread_count);
        data->set_number_of_rays(100000);
        data->set_max_rays_traced(1000000);
        qDebug() << data->get_number_of_rays()
                 << data->get_max_number_rays_traced();

        SOLTRACE_SECTION(initialize(), 0, "Starting simulation");


        SOLTRACE_SECTION(setup_simulation(data.get()), 0, "Setup simulation");

        qDebug() << Q_FUNC_INFO << "setup complete";

        // run simulation will indeed run, but we need to stuff it into another
        // thread so we can poll status.

        auto run_future =
            QtConcurrent::run(execute_solve_with, current_runner.get());

        double last_progress = -1;

        while (true) {
            if (run_future.isFinished()) { break; }

            // Normally polling would be The Wrong Thing, but the progress stuff
            // requires active checking
            QThread::sleep(std::chrono::milliseconds { 500 });

            double progress = -1;

            // If cancelled, we set the flag, wait, and bail
            if (promise.isCanceled()) {
                promise.setProgressValueAndText(100, "Cancelling...");

                // User cancelled, set flag
                current_runner->cancel_simulation();

                // Wait for completion
                run_future.result();

                // bail
                promise.emplaceResult(QStringLiteral("Cancelled"));
                qDebug() << Q_FUNC_INFO << "cancelled";
                return;
            }

            // Not cancelled, check and see how things are going
            current_runner->status_simulation(&progress);

            qDebug() << "raw" << progress;

            // assuming progress is 0-1, TODO: Check
            // It is, but there is a bug in the lib. HACK
            progress = std::clamp(progress / 10., 0.0, 1.0);


            if (last_progress != progress) {
                // we have reserved progress points 10 to 90 for sim run

                promise.setProgressValueAndText(std::lerp(10, 90, progress),
                                                "Running...");
                last_progress = progress;
                qDebug() << "sim progress " << progress;
            }
        }

        auto run_result = run_future.result();

        auto end_instant = std::chrono::high_resolution_clock::now();

        qDebug() << Q_FUNC_INFO << "Run complete in: "
                 << std::chrono::duration<double>(end_instant - start_instant)
                        .count();

        switch (run_result) {
        case SolTrace::Runner::RunnerStatus::CANCEL:
            promise.setProgressValueAndText(100, "Cancelled");
            return;
        case SolTrace::Runner::RunnerStatus::ERROR:
            promise.setProgressValueAndText(100, "Run failed");
            return;
        case SolTrace::Runner::RunnerStatus::RUNNING:
            // we really shouldnt get here
            qWarning("Sim result declares running, but was finished??");
            break;
        case SolTrace::Runner::RunnerStatus::SUCCESS: break;
        case SolTrace::Runner::RunnerStatus::TIMEOUT:
            promise.setProgressValueAndText(100, "Run failed: timeout");
            return;
        default:
            promise.setProgressValueAndText(100, "Run failed: unknown");
            return;
        }

        qDebug() << Q_FUNC_INFO << "Build result database";

        auto ret = std::make_shared<ResultDB>();

        SOLTRACE_SECTION(
            report_simulation(&(ret->result), 100), 90, "Report simulation");

        construct_result(promise, ret, data);

    } catch (std::exception& e) {
        promise.emplaceResult(QString(e.what()));
        return;
    }
}
