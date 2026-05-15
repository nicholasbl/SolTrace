#include "simulation_module.h"

#include <thread>

namespace SolTrace::GUI::App {

SimulationRunnerModel::SimulationRunnerModel(QObject* parent)
    : StructModelAdapter { parent } {
    store_push_append(SimulationRunnerRecord {
        .name   = "CPU Runner",
        .runner = SimulationModule::CPU,
    });

#ifdef SOLTRACE_HAS_EMBREE_RUNNER
    store_push_append(SimulationRunnerRecord {
        .name   = "Embree Runner",
        .runner = SimulationModule::Embree,
    });
#endif

#ifdef SOLTRACE_HAS_OPTIX_RUNNER
    store_push_append(SimulationRunnerRecord {
        .name   = "GPU Runner",
        .runner = SimulationModule::GPU,
    });
#endif
}

SimulationModule::Runner SimulationRunnerModel::runner_at(int index) const {
    auto record = get_at(index);
    if (!record) return SimulationModule::CPU;

    return record->runner;
}

int SimulationRunnerModel::index_of(SimulationModule::Runner runner) const {
    int index = 0;
    for (auto const& record : *this) {
        if (record.runner == runner) return index;
        ++index;
    }

    return 0;
}

void SimulationModule::update_result_world(db::SimulationResultPtr results) {
    auto* database =
        results ? const_cast<db::Database*>(results->database.get()) : nullptr;
    m_world_geometry_model->reset(database);
}

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

    if (!results) {
        emit notify(
            ANotification::error("Simulation finished without results."));
        return;
    }

    m_completed_sims.push_back(results);
    m_results->append_result(results);
    m_current_result = results;
    set_current_simulation_result_name(results->database->name());

    qDebug() << Q_FUNC_INFO << "publish";
    emit new_results(results);
}

SimulationModule::SimulationModule(QObject* parent)
    : QObject { parent },
      m_status(new StatusComponent(this)),
      m_runners(new SimulationRunnerModel(this)),
      m_results(new db::SimulationResultModel(this)),
      m_world_geometry_model(new db::WorldGeometryModel(this)) {

    auto thread_count = std::thread::hardware_concurrency();
    set_max_threads(thread_count <= 0 ? 1 : thread_count);

    connect(this,
            &SimulationModule::new_results,
            this,
            &SimulationModule::update_result_world);

    qDebug() << Q_FUNC_INFO;
}

SimulationModule::~SimulationModule() {
    qDebug() << Q_FUNC_INFO;
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
    connect(
        m_running, &RunningJob::finished, m_running, &RunningJob::deleteLater);
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

void SimulationModule::select_result(int index) {
    auto result = m_results->result_at(index);
    if (!result) return;

    m_current_result = result;
    set_current_simulation_result_name(result->database->name());
    emit new_results(result);
}

void SimulationModule::duplicate_current_result_for_edit() {
    if (!m_current_result) return;

    emit edit_result_copy_requested(m_current_result);
}

} // namespace SolTrace::GUI::App
