#include "ray_geometry.h"

#include "analysis/ray_volume_raster.h"
#include "utilities/math_utility.h"

namespace analysis {

struct LineVertex {
    QVector3D position;
    QVector2D uv;
};

void RayGeometry::rebuild_geometry() {
    if (!m_database) { return; }

    clear();

    size_t vertex_count = 0;

    {

        for (auto iter = m_database->result.get_ray_record_iteratior();
             !m_database->result.is_at_end(iter);
             ++iter) {

            vertex_count += (*iter)->interactions.size();
        }
    }

    qDebug() << Q_FUNC_INFO << vertex_count;

    std::vector<LineVertex> verts;
    std::vector<uint32_t>   index;
    verts.reserve(vertex_count);
    index.reserve(vertex_count * 2); // close enough


    size_t ray_limit = m_database->result.get_number_of_records() *
                       (this->show_percent() / 100.);

    {
        size_t ray_number = 0;

        for (auto iter = m_database->result.get_ray_record_iteratior();
             !m_database->result.is_at_end(iter);
             ++iter) {

            if (ray_limit == 0) { break; }

            // qDebug() << "Ray" << ray_number;

            ray_limit -= 1;
            ray_number += 1;


            auto& this_ray = (**iter);

            // since we are now filtering, we cannot use interaction counts
            size_t ray_interaction_count = 0;
            double total_ray_distance    = 0.0;

            QVector3D last_point = {};

            // first compute an idea of the total ray distance
            for (auto const& interaction : this_ray.interactions) {

                if (m_exclude_events.contains(interaction->event)) { continue; }

                auto p = convert(interaction->location.data);

                // if this is not the first
                if (ray_interaction_count > 0) {
                    // record delta
                    total_ray_distance += (p - last_point).length();
                } else {
                    // this is the first. just record
                    last_point = p;
                }

                ray_interaction_count += 1;
            }

            if (total_ray_distance == 0.0) { total_ray_distance = 1.0; }

            // qDebug() << "Distance" << total_ray_distance;

            // Reset counter
            ray_interaction_count       = 0;
            double current_ray_distance = 0.0;

            for (auto const& interaction : this_ray.interactions) {

                if (m_exclude_events.contains(interaction->event)) { continue; }

                auto p = convert(interaction->location.data);

                // qDebug() << "Point" << p << "type" <<
                // (int)interaction->event;

                // if this is not the first
                if (ray_interaction_count > 0) {
                    // record delta
                    current_ray_distance += (p - last_point).length();
                } else {
                    // this is the first. just record
                    last_point = p;
                }

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

                ray_interaction_count += 1;
            }
        }
    }


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
    setBounds(m_database->bounds_min, m_database->bounds_max);
    setPrimitiveType(QQuick3DGeometry::PrimitiveType::Lines);
    update();
}

RayGeometry::RayGeometry(QQuick3DObject* parent) : QQuick3DGeometry(parent) {

    // WATCH OUT HERE
    m_ray_volume = new RayVolume();

    connect(this,
            &RayGeometry::show_percent_changed,
            this,
            &RayGeometry::rebuild_geometry);
}

void RayGeometry::set_database(std::shared_ptr<ResultDB>&& data) {
    qDebug() << "New ray geometry database";
    m_database = std::move(data);
    rebuild_geometry();
}

} // namespace analysis
