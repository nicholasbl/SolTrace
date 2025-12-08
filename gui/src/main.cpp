#include "job_run_process.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>


int main(int argc, char* argv[]) {

    // Check if we are a worker. If so, this function will not return.
    check_if_process_worker(argc, argv);

    qputenv("QT_QUICK_CONTROLS_MATERIAL_VARIANT", "Dense");
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("SolTraceProto", "Main");

    return app.exec();
}
