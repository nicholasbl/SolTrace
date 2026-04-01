#pragma once

#include "database/database.h"
#include "job_control/job_run.h"
#include "utilities/notification.h"
#include "utilities/qt_helpers.h"

#include "module_common.h"

#include <QObject>
#include <QQmlEngine>

namespace SolTrace::GUI::App {

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
    QML_ELEMENT

    QPointer<RunningJob> m_running;

private slots:
    void job_done();

public:
    explicit SimulationModule(QObject* parent = nullptr);

    enum class Camera { WASD, Orbital };

    enum class Perspective { Normal, Orthographic };

    Q_ENUM(Camera)
    Q_ENUM(Perspective)

    QOBJECT_WRITABLE_PROPERTY(db::Database, current_database)
    QOBJECT_READONLY_PROPERTY(StatusComponent, status);

    Q_READONLY_PROPERTY(bool, is_running) /// Is a simulation being run?
    Q_READONLY_PROPERTY(double, progress) /// Ray tracing progress, 0.0 to 1.0
    Q_READONLY_PROPERTY(
        QString,
        current_stage) /// e.g. "Initializing", "Ray tracing", "Complete"
    Q_READONLY_PROPERTY(QDateTime, last_run_time)
    Q_READONLY_PROPERTY(double, elapsed_seconds)

    Q_WRITABLE_PROPERTY(Camera, camera, Camera::Orbital)
    Q_WRITABLE_PROPERTY(Perspective, perspective, Perspective::Normal)


public slots:
    void run();
    // void pause(); // no executor support for pause or resume
    // void resume();
    void cancel();

signals:
    void new_results(std::shared_ptr<ResultDB>);
    void notify(ANotification);
};


} // namespace SolTrace::GUI::App
