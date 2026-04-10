#pragma once

#include "database/database.h"
#include "job_control/job_run.h"
#include "utilities/notification.h"
#include "utilities/qt_helpers.h"

#include "module_common.h"

#include <QObject>
#include <QQmlEngine>

namespace SolTrace::GUI::App {

// TODO: Track added simulation results and allow deletion!

/**
 * @class SimulationModule
 * @brief Simulation execution and progress tracking module.
 *
 * Mediates between QML controls and the job runner.
 * Exposes progress, timing metadata, and execution control to QML.
 *
 * start(), stop(), and pause() delegate to the backend after validating
 * that all required configuration modules are in a Ready or Complete state.
 *
 * QML access pattern: App.simulation.start()
 */
class SimulationModule : public QObject {
    Q_OBJECT

    QPointer<RunningJob> m_running;

private slots:
    void job_done();

public:
    explicit SimulationModule(QObject* parent = nullptr);

    QOBJECT_WRITABLE_PROPERTY(db::Database, current_database)
    QOBJECT_READONLY_PROPERTY(StatusComponent, status);

    Q_WRITABLE_PROPERTY(uint32_t, ray_count, 10000);
    Q_WRITABLE_PROPERTY(uint32_t, max_ray_count, 100000);
    Q_WRITABLE_PROPERTY(uint32_t, max_threads, 10);

    /// Is a simulation being run?
    Q_READONLY_PROPERTY(bool, is_running)

    /// Ray tracing progress, 0 to 100
    Q_READONLY_PROPERTY(int, progress)

    /// e.g. "Initializing", "Ray tracing", "Complete"
    Q_READONLY_PROPERTY(QString, current_stage)
    Q_READONLY_PROPERTY(QDateTime, last_run_time)
    Q_READONLY_PROPERTY(double, elapsed_seconds)

public slots:
    void run();
    // void pause(); // no executor support for pause or resume
    // void resume();
    void cancel();

signals:
    void new_results(db::SimulationResult*);
    void notify(ANotification);
};


} // namespace SolTrace::GUI::App
