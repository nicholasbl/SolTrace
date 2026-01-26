#include "job_control/job_run_process.h"
#include <QFontDatabase>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QFile>
#include <QQuickWindow>
#include <QQmlContext>

int main(int argc, char* argv[]) {
    // Check if we are a worker. If so, this function will not return.
    check_if_process_worker(argc, argv);

    qputenv("QT_QUICK_CONTROLS_MATERIAL_VARIANT", "Dense");

    QApplication app(argc, argv);

    // Load fonts
    QFontDatabase::addApplicationFont(":/fonts/assets/fonts/computer-modern/cmunrm.ttf");
    QFontDatabase::addApplicationFont(":/fonts/assets/fonts/computer-modern/cmunbx.ttf");
    QFontDatabase::addApplicationFont(":/fonts/assets/fonts/computer-modern/cmunti.ttf");
    QFontDatabase::addApplicationFont(":/fonts/assets/fonts/computer-modern/cmunbi.ttf");

    QQmlApplicationEngine engine;

    // Initialize context properties as null/undefined before loading QML
    engine.rootContext()->setContextProperty("globalWindow", QVariant());
    engine.rootContext()->setContextProperty("globalScene", QVariant());

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.loadFromModule("SolTraceProto", "Main");

    if (!engine.rootObjects().isEmpty()) {
        QObject* rootObject = engine.rootObjects().first();
        QQuickWindow* window = qobject_cast<QQuickWindow*>(rootObject);
        if (window) {
            engine.rootContext()->setContextProperty("globalWindow", QVariant::fromValue(window));

            QObject* scene = window->findChild<QObject*>("simulationScene");
            if (scene) {
                engine.rootContext()->setContextProperty("globalScene", QVariant::fromValue(scene));
            }
        }
    }

    return app.exec();
}
