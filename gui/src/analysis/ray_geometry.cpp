#include "ray_geometry.h"

#include "analysis/ray_volume_raster.h"
#include "utilities/math_utility.h"

#include <magic_enum/magic_enum.hpp>
#include <algorithm>
#include <cmath>

namespace analysis {

namespace {

constexpr quint64 DEFAULT_VISIBLE_RAY_COUNT = 10000;

float percent_for_ray_count(quint64 count, quint64 available) {
    if (available == 0) return 0.0f;
    return static_cast<float>(count * 100.0 / available);
}

} // namespace

EventTypeContainer::EventTypeContainer(
    std::initializer_list<db::RayEventType> l)
    : events(l) { }

EventTypeContainer::EventTypeContainer(QStringList l) {
    for (auto const& item : l) {
        auto str = item.toUpper().toStdString();

        auto maybe_enum = magic_enum::enum_cast<db::RayEventType>(str);

        if (!maybe_enum) {
            qDebug() << "Unknown enum name" << item;
            continue;
        }

        events.insert(*maybe_enum);
    }
}

QStringList EventTypeContainer::to_list() const {
    QStringList ret;

    for (auto item : events) {
        ret << QString(magic_enum::enum_name(item).data()).toLower();
    }

    return ret;
}

struct LineVertex {
    QVector3D position;
    QVector2D uv;
};

void RayGeometry::rebuild_geometry() {
    qDebug() << Q_FUNC_INFO << "Start";
    if (!m_database) {
        qDebug() << Q_FUNC_INFO << "No database";
        return;
    }

    clear();

    const auto effective_percent = std::clamp(this->show_percent(), 0.0f, 100.0f);
    const auto requested_rays =
        static_cast<double>(m_database->records.size()) *
        static_cast<double>(effective_percent) / 100.0;
    const size_t ray_limit = std::min(
        m_database->records.size(),
        static_cast<size_t>(std::llround(requested_rays)));

    size_t vertex_count = 0;
    size_t counted_rays = 0;
    for (auto const& rec : m_database->records) {
        if (counted_rays >= ray_limit) break;
        vertex_count += rec.events.size();
        counted_rays += 1;
    }

    qDebug() << Q_FUNC_INFO << vertex_count;

    std::vector<LineVertex> verts;
    std::vector<uint32_t>   index;
    verts.reserve(vertex_count);
    index.reserve(vertex_count * 2); // close enough

    {
        size_t ray_number = 0;
        size_t rays_remaining = ray_limit;

        for (auto const& ray : m_database->records) {

            if (rays_remaining == 0) { break; }

            // qDebug() << "Ray" << ray_number;

            rays_remaining -= 1;
            ray_number += 1;


            // since we are now filtering, we cannot use interaction counts
            size_t ray_interaction_count = 0;
            double total_ray_distance    = 0.0;

            QVector3D last_point = {};

            // first compute an idea of the total ray distance
            for (auto const& interaction : ray.events) {

                if (!m_include_events.events.contains(interaction.event)) {
                    continue;
                }

                auto p = convert(interaction.location);

                // if this is not the first
                if (ray_interaction_count > 0) {
                    // record delta
                    total_ray_distance += (p - last_point).length();
                }

                last_point = p;
                ray_interaction_count += 1;
            }

            if (total_ray_distance == 0.0) { total_ray_distance = 1.0; }

            // qDebug() << "Distance" << total_ray_distance;

            // Reset counter
            ray_interaction_count       = 0;
            double current_ray_distance = 0.0;

            for (auto const& interaction : ray.events) {

                if (!m_include_events.events.contains(interaction.event)) {
                    continue;
                }

                auto p = convert(interaction.location);

                // qDebug() << "Point" << p << "type" <<
                // (int)interaction->event;

                // if this is not the first
                if (ray_interaction_count > 0) {
                    // record delta
                    current_ray_distance += (p - last_point).length();
                }

                last_point = p;
                ray_interaction_count += 1;
                verts.push_back({
                    .position = p,
                    .uv =
                        QVector2D(current_ray_distance / total_ray_distance, 0),
                });


                // install index: connect consecutive vertices within this ray
                if (ray_interaction_count > 1) {
                    auto cur  = static_cast<uint32_t>(verts.size() - 1);
                    auto prev = static_cast<uint32_t>(verts.size() - 2);
                    index.push_back(prev);
                    index.push_back(cur);
                }
            }
        }
    }

    qDebug() << Q_FUNC_INFO << "New buffers ready";

    auto vertex_buffer = QByteArray(reinterpret_cast<const char*>(verts.data()),
                                    verts.size() * sizeof(LineVertex));
    auto index_buffer  = QByteArray(reinterpret_cast<const char*>(index.data()),
                                   index.size() * sizeof(uint32_t));

    addAttribute(QQuick3DGeometry::Attribute::PositionSemantic,
                 0,
                 QQuick3DGeometry::Attribute::ComponentType::F32Type);

    addAttribute(QQuick3DGeometry::Attribute::TexCoord0Semantic,
                 3 * sizeof(float),
                 QQuick3DGeometry::Attribute::ComponentType::F32Type);

    addAttribute(QQuick3DGeometry::Attribute::IndexSemantic,
                 0,
                 QQuick3DGeometry::Attribute::ComponentType::U32Type);

    setStride(sizeof(LineVertex));
    setVertexData(vertex_buffer);
    setIndexData(index_buffer);
    setBounds(QVector3D(m_database->bounds_min.x,
                        m_database->bounds_min.y,
                        m_database->bounds_min.z),
              QVector3D(m_database->bounds_max.x,
                        m_database->bounds_max.y,
                        m_database->bounds_max.z));
    setPrimitiveType(QQuick3DGeometry::PrimitiveType::Lines);

    qDebug() << Q_FUNC_INFO << "Update";
    update();
}

void RayGeometry::inclusion_list_update() {
    qDebug() << Q_FUNC_INFO << "List changed";
    m_include_events = EventTypeContainer(event_include());

    rebuild_geometry();
}

RayGeometry::RayGeometry(QQuick3DObject* parent) : QQuick3DGeometry(parent) {

    m_include_events = EventTypeContainer({
        db::RayEventType::ABSORB,
        db::RayEventType::REFLECT,
        db::RayEventType::TRANSMIT,
    });

    set_event_include(m_include_events.to_list());

    connect(this,
            &RayGeometry::event_include_changed,
            this,
            &RayGeometry::inclusion_list_update);

    connect(this,
            &RayGeometry::show_percent_changed,
            this,
            &RayGeometry::rebuild_geometry);
}

void RayGeometry::set_results(db::SimulationResultPtr data) {
    qDebug() << Q_FUNC_INFO << "New ray geometry database";
    m_database = std::move(data);
    const auto available =
        m_database ? static_cast<quint64>(m_database->records.size()) : 0;
    set_available_rays(available);

    const auto default_percent = percent_for_ray_count(
        std::min(DEFAULT_VISIBLE_RAY_COUNT, available), available);
    const bool percent_changed = show_percent() != default_percent;
    set_show_percent(default_percent);
    if (!percent_changed) rebuild_geometry();
}

} // namespace analysis
