#include "simulation_module.h"

namespace SolTrace::GUI::App {


void SimulationModule::job_done() {
    qDebug() << Q_FUNC_INFO;

    auto* from = qobject_cast<RunningJob*>(sender());
    if (!from) {
        qCritical() << Q_FUNC_INFO << "bad cast";
        return;
    }

    if (m_running != from) {
        qCritical() << Q_FUNC_INFO << m_running << from;
        return;
    }

    set_is_running(false);

    m_running = nullptr;

    auto results = from->take();

    emit new_results(results);
}

SimulationModule::SimulationModule(QObject* parent)
    : QObject { parent }, m_status(new StatusComponent(this)) { }

void SimulationModule::run() {
    qDebug() << Q_FUNC_INFO;
    if (!m_current_database) return;

    if (m_running) {
        emit notify(
            ANotification::error("A running job is already in progress."));
        return;
    }

    qDebug() << Q_FUNC_INFO << "Launch";
    m_running = new RunningJob(
        m_current_database->export_to_simdata(), RunType::Thread, this);

    connect(m_running,
            &RunningJob::progress_update,
            this,
            &SimulationModule::set_progress);
    connect(m_running,
            &RunningJob::progress_text_update,
            this,
            &SimulationModule::set_current_stage);

    connect(
        m_running, &RunningJob::finished, this, &SimulationModule::job_done);
    connect(m_running, &RunningJob::finished, this, &RunningJob::deleteLater);
    connect(this, &QObject::destroyed, m_running, &RunningJob::cancel);

    set_is_running(true);
}
// void SimulationModule::pause() {

// }
// void SimulationModule::resume() {

// }
void SimulationModule::cancel() {
    if (m_running) { m_running->cancel(); }
}

} // namespace SolTrace::GUI::App
