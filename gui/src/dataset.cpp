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
    set_shape(ptr->get_shape());
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
    m_ray_source->set_shape(
        shape(), sigma(), half_width(), 0.0, std::move(ua), std::move(ui));
}

// =============================================================================

bool ElementTableModel::_can_append_new(QVariant const&) {
    return false;
}

void ElementTableModel::_append_new(QVariant) { }

bool ElementTableModel::_can_delete_at(size_t, size_t) {
    return true;
}

void ElementTableModel::_delete_at(size_t place, size_t count) {
    for (auto i = 0; i < count; i++) {
        m_data->remove_element(m_known_keys[place + i]);
    }

    m_known_keys.remove(place, count);
}

void ElementTableModel::_clear() {
    // no fast clear
    for (auto k : std::as_const(m_known_keys)) {
        m_data->remove_element(k);
    }

    m_known_keys.clear();
}

ElementTableModel::ElementTableModel(SimDataPtr ptr, QObject* parent)
    : HashContainerModel(parent),
      m_data(ptr),
      m_surface_geometries(new SurfaceGeometryListModel(this)),
      m_ray_geometry(new RayGeometry()) {
    // TODO implementing...

    add_property({
        .display_name = "enabled",
        .getter       = [this](size_t index) -> QVariant {
            return m_data->get_element(this->m_known_keys[index])->is_enabled();
        },
        .setter =
            [this](size_t index, QVariant value) {
                bool        enabled = value.toBool();
                auto const& e = m_data->get_element(this->m_known_keys[index]);
                if (enabled) {
                    e->enable();
                } else {
                    e->disable();
                }
                return true;
            },
    });

    add_property({
        .display_name = "name",
        .getter       = [this](size_t index) -> QVariant {
            return QString::fromStdString(
                m_data->get_element(this->m_known_keys[index])->get_name());
        },
        .setter =
            [this](size_t index, QVariant value) {
                auto        new_name = value.toString();
                auto const& e = m_data->get_element(this->m_known_keys[index]);
                e->set_name(new_name.toStdString());
                return true;
            },
    });

    add_property({
        .display_name = "origin",
        .getter       = [this](size_t index) -> QVariant {
            return convert(m_data->get_element(this->m_known_keys[index])
                               ->get_origin_ref());
        },
        .setter =
            [this](size_t index, QVariant value) {
                auto        v = value.value<QVector3D>();
                auto const& e = m_data->get_element(this->m_known_keys[index]);
                e->set_origin(convert(v));
                return true;
            },
    });

    add_property({
        .display_name = "global origin",
        .getter       = [this](size_t index) -> QVariant {
            return convert(m_data->get_element(this->m_known_keys[index])
                               ->get_origin_global());
        },
    });


    // scan initial

    m_known_keys.reserve(m_data->get_number_of_elements());

    for (auto iter = m_data->get_const_iterator(); !m_data->is_at_end(iter);
         ++iter) {
        m_known_keys.push_back(iter->first);
    }

    int singles = 0;
    int composites = 0;

    for (auto iter = m_data->get_iterator();!m_data->is_at_end(iter); iter++)
    {
        if (iter->second->is_single())
        {
            m_surface_geometries->push_back(std::make_shared<SurfaceGeometry>(iter->second));
            singles++;
        }
        else
        {
            composites++;
        }
    }

    qDebug() << "Added" << m_known_keys.size() << "elements (" << singles << " singles, " << composites << "composites)";
}

ElementTableModel::~ElementTableModel() {
    // TODO better ownership model
    m_ray_geometry->deleteLater();
}

void ElementTableModel::sim_done() {
    qDebug() << "Collecting simulation results...";
    auto* job = qobject_cast<RunningJob*>(sender());

    if (!job) return;

    auto result = job->take();

    m_ray_geometry->set_database(std::move(result));
}

void ElementTableModel::run_simulation() {
    if (sim_running()) return;

    set_sim_running(true);

    // hack hack hack
    auto* job = new RunningJob(m_data, RunType::Thread, this);

    connect(job, &RunningJob::progress_text_update, this, [](QString text) {
        qDebug() << "JOB:" << text;
    });

    connect(job, &RunningJob::error, this, [](QString text) {
        qCritical() << "JOB FAIL:" << text;
    });

    connect(job, &RunningJob::finished, this, &ElementTableModel::sim_done);
}

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

void Data::mark_changed() {
    set_modified(true);
}

Data::Data(SimDataPtr ptr, QObject* parent)
    : QObject(parent),
      m_ray_source_model(new RaySourceModel(ptr, this)),
      m_element_model(new ElementTableModel(ptr, this)) {
    auto changed_slot = metaObject()->indexOfSlot("mark_changed()");

    Q_ASSERT(changed_slot >= 0);

    auto changed_meta_slot = metaObject()->method(changed_slot);

    Q_ASSERT(changed_meta_slot.isValid());

    recursive_wire_changed(m_ray_source_model, [&](auto o, auto sender) {
        connect(o, sender, this, changed_meta_slot);
    });

    connect(m_element_model,
            &ElementTableModel::dataChanged,
            this,
            &Data::mark_changed);
}
