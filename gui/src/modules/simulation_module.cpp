#include "simulation_module.h"

#include <thread>

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
    : QObject { parent }, m_status(new StatusComponent(this)) {

    auto thread_count = std::thread::hardware_concurrency();
    set_max_threads(thread_count <= 0 ? 1 : thread_count);
}

void SimulationModule::run() {
    qDebug() << Q_FUNC_INFO;
    if (!m_current_database) return;

    if (m_running) {
        emit notify(
            ANotification::error("A running job is already in progress."));
        return;
    }

    qDebug() << Q_FUNC_INFO << "Launch";
    auto sim_data = m_current_database->export_to_simdata();

    if (!sim_data) {
        emit notify(
            ANotification::error("Unable to pack simulation database."));
        return;
    }

    sim_data->data->set_number_of_rays(m_ray_count);
    sim_data->data->set_max_rays_traced(m_max_ray_count);

    m_running = new RunningJob(sim_data, RunType::Thread, m_max_threads, this);

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
