#include "dataset.h"

#include "simulation_data_export.hpp"

#include <QVector3D>

RaySourceModel::RaySourceModel(SimDataPtr data, QObject* parent)
    : QObject(parent) {
    // try to find the first ray source

    auto ptr = data->get_ray_source();

    if (!ptr) {
        ptr = SD::make_ray_source<SD::Sun>();
        data->add_ray_source(ptr);
    }

    m_ray_source = std::dynamic_pointer_cast<SD::Sun>(ptr);

    if (!m_ray_source) {
        qWarning() << "Non-sun ray sources are not yet supported!";
        return;
    }

    set_position(convert(ptr->get_position()));
    // set_shape(ptr->get_shape());
    set_sigma(ptr->get_sigma());
    set_half_width(ptr->get_half_width());

    std::vector<double> angles, intensities;
    ptr->get_user_data(angles, intensities);

    set_user_angle(QVector<double>(angles.begin(), angles.end()));
    set_user_intensity(QVector<double>(intensities.begin(), intensities.end()));
}

void RaySourceModel::commit() {
    if (!m_ray_source) return;

    m_ray_source->set_position(convert(position()));

    auto ua = std::vector<double>(m_user_angle.begin(), m_user_angle.end());
    auto ui =
        std::vector<double>(m_user_intensity.begin(), m_user_intensity.end());

    // TODO: ADD IN CSR
    // m_ray_source->set_shape(
    //     shape(), sigma(), half_width(), 0.0, std::move(ua), std::move(ui));
}

// =============================================================================

// void ElementTableModel::sim_done() {
//     qDebug() << "Collecting simulation results...";
//     auto* job = qobject_cast<RunningJob*>(sender());

//     if (!job) return;

//     auto result = job->take();

//     m_ray_geometry->set_database(std::move(result));
// }

// void ElementTableModel::run_simulation() {
//     if (sim_running()) return;

//     set_sim_running(true);

//     // hack hack hack
//     auto* job = new RunningJob(m_data, RunType::Thread, this);

//     connect(job, &RunningJob::progress_text_update, this, [](QString text) {
//         qDebug() << "JOB:" << text;
//     });

//     connect(job, &RunningJob::error, this, [](QString text) {
//         qCritical() << "JOB FAIL:" << text;
//     });

//     connect(job, &RunningJob::finished, this, &ElementTableModel::sim_done);
// }

// =============================================================================

template <class F>
void recursive_wire_changed(QObject* p, F const& f) {
    if (!p) return;

    auto* m = p->metaObject();

    for (int p_i = 0; p_i < m->propertyCount(); p_i++) {
        auto prop = m->property(p_i);

        auto notify = prop.notifySignal();

        if (!notify.isValid()) continue;

        f(p, notify);
    }

    for (auto c : p->children()) {
        recursive_wire_changed(c, f);
    }
}

void LocalData::mark_changed() {
    // set_modified(true);
}

// Data::Data(SimDataPtr ptr, QObject* parent)
//     : QObject(parent),
//       m_ray_source_model(new RaySourceModel(ptr, this)),
//       m_element_model(new ElementTableModel(ptr, this)) {
//     auto changed_slot = metaObject()->indexOfSlot("mark_changed()");

//     Q_ASSERT(changed_slot >= 0);

//     auto changed_meta_slot = metaObject()->method(changed_slot);

//     Q_ASSERT(changed_meta_slot.isValid());

//     recursive_wire_changed(m_ray_source_model, [&](auto o, auto sender) {
//         connect(o, sender, this, changed_meta_slot);
//     });

//     connect(m_element_model,
//             &ElementTableModel::dataChanged,
//             this,
//             &Data::mark_changed);
// }
