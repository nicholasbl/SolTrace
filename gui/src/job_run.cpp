#include "job_run.h"

#include "dataset.h"
#include "job_run_process.h"
#include "job_run_thread.h"
#include "native_runner/native_runner.hpp"
#include "simulation_result.hpp"
#include "simulation_runner.hpp"
#include "utility.h"

#include <QDir>
#include <QException>
#include <QFutureWatcher>
#include <QGuiApplication>
#include <QPromise>
#include <QtConcurrentRun>


// Native only for the moment


// =============================================================================

using Vec3i = std::array<int, 3>;

inline Vec3i floor(QVector3D x) {
    return { static_cast<int>(std::floor(x.x())),
             static_cast<int>(std::floor(x.y())),
             static_cast<int>(std::floor(x.z())) };
}

inline Vec3i ceil(QVector3D x) {
    return { static_cast<int>(std::ceil(x.x())),
             static_cast<int>(std::ceil(x.y())),
             static_cast<int>(std::ceil(x.z())) };
}

Vec3i world_to_voxel(QVector3D const& p,
                     QVector3D const& origin,
                     float            cellSize) {
    auto rel = (p - origin) / cellSize;
    return floor(rel);
}

template <typename T>
void raster_segment(Grid3D<T>&       grid,
                    QVector3D const& p0_grid,
                    QVector3D const& p1_grid) {
    // Direction in grid space
    QVector3D d = p1_grid - p0_grid;

    // Start and end voxels (integer indices)
    int x = static_cast<int>(std::floor(p0_grid.x()));
    int y = static_cast<int>(std::floor(p0_grid.y()));
    int z = static_cast<int>(std::floor(p0_grid.z()));

    int x1 = static_cast<int>(std::floor(p1_grid.x()));
    int y1 = static_cast<int>(std::floor(p1_grid.y()));
    int z1 = static_cast<int>(std::floor(p1_grid.z()));

    auto in_bounds = [&](int xi, int yi, int zi) {
        return xi >= 0 && yi >= 0 && zi >= 0 &&
               xi < static_cast<int>(grid.size_x()) &&
               yi < static_cast<int>(grid.size_y()) &&
               zi < static_cast<int>(grid.size_z());
    };

    // If both endpoints are outside and you don't want to handle clipping,
    // you can early out. More robust is to clip the segment to the grid box.
    if (!in_bounds(x, y, z) && !in_bounds(x1, y1, z1)) return;

    // Step direction per axis (-1, 0, or +1)
    int stepX = (d.x() > 0.f) ? 1 : (d.x() < 0.f ? -1 : 0);
    int stepY = (d.y() > 0.f) ? 1 : (d.y() < 0.f ? -1 : 0);
    int stepZ = (d.z() > 0.f) ? 1 : (d.z() < 0.f ? -1 : 0);

    float tMaxX, tMaxY, tMaxZ;
    float tDeltaX, tDeltaY, tDeltaZ;

    auto INF = std::numeric_limits<float>::infinity();

    // X axis
    if (stepX != 0) {
        float nextVoxelBoundary = (stepX > 0) ? (static_cast<float>(x) + 1.0f)
                                              : static_cast<float>(x);
        tMaxX                   = (nextVoxelBoundary - p0_grid.x()) / d.x();
        tDeltaX                 = 1.0f / std::fabs(d.x());
    } else {
        tMaxX   = INF;
        tDeltaX = INF;
    }

    // Y axis
    if (stepY != 0) {
        float nextVoxelBoundary = (stepY > 0) ? (static_cast<float>(y) + 1.0f)
                                              : static_cast<float>(y);
        tMaxY                   = (nextVoxelBoundary - p0_grid.y()) / d.y();
        tDeltaY                 = 1.0f / std::fabs(d.y());
    } else {
        tMaxY   = INF;
        tDeltaY = INF;
    }

    // Z axis
    if (stepZ != 0) {
        float nextVoxelBoundary = (stepZ > 0) ? (static_cast<float>(z) + 1.0f)
                                              : static_cast<float>(z);
        tMaxZ                   = (nextVoxelBoundary - p0_grid.z()) / d.z();
        tDeltaZ                 = 1.0f / std::fabs(d.z());
    } else {
        tMaxZ   = INF;
        tDeltaZ = INF;
    }

    // Make sure start voxel is inside before touching the grid
    if (in_bounds(x, y, z)) {
        grid(static_cast<size_t>(x),
             static_cast<size_t>(y),
             static_cast<size_t>(z)) += T(1);
    }

    // Traverse until we reach the voxel containing p1
    while (x != x1 || y != y1 || z != z1) {
        // Advance to next voxel boundary in the dimension with smallest tMax
        if (tMaxX < tMaxY) {
            if (tMaxX < tMaxZ) {
                x += stepX;
                tMaxX += tDeltaX;
            } else {
                z += stepZ;
                tMaxZ += tDeltaZ;
            }
        } else {
            if (tMaxY < tMaxZ) {
                y += stepY;
                tMaxY += tDeltaY;
            } else {
                z += stepZ;
                tMaxZ += tDeltaZ;
            }
        }

        if (!in_bounds(x, y, z)) break;

        grid(static_cast<size_t>(x),
             static_cast<size_t>(y),
             static_cast<size_t>(z)) += T(1);
    }
}

// =============================================================================

RunningJob::RunningJob(SimDataPtr data, RunType type, QObject* parent)
    : QObject(parent) {

    void (*f_ptr)(QPromise<SimResult>& promise, SimDataPtr data);

    switch (type) {
    case RunType::Thread: f_ptr = execute_thread_runner; break;
    case RunType::Process: f_ptr = execute_process_runner; break;
    }

#ifdef Q_WASM
    f_ptr = execute_thread_runner;
#endif

    // TEMPORARY HACK WHILE WE FIX PROCESS STUFF
    f_ptr = execute_thread_runner;

    auto future = QtConcurrent::run(f_ptr, data);

    auto watcher = new QFutureWatcher<SimResult>();

    m_watcher = watcher;

    connect(watcher, &QFutureWatcher<Result>::finished, this, [this]() {
        auto watcher = ((QFutureWatcher<SimResult>*)(this->m_watcher));

        auto res = watcher->result();

        std::visit(
            overloaded {
                [this](ResultPtr& ptr) {
                    this->m_result = std::move(ptr);

                    emit this->finished();
                },
                [this](QString error_text) { emit this->error(error_text); },
            },
            res);
    });

    connect(watcher,
            &QFutureWatcher<Result>::finished,
            this,
            &QObject::deleteLater);

    connect(watcher,
            &QFutureWatcher<Result>::canceled,
            this,
            &QObject::deleteLater);

    connect(watcher,
            &QFutureWatcher<Result>::progressValueChanged,
            this,
            &RunningJob::progress_update);
    connect(watcher,
            &QFutureWatcher<Result>::progressTextChanged,
            this,
            &RunningJob::progress_text_update);

    connect(watcher, &QFutureWatcher<Result>::canceled, this, [this]() {
        emit this->error("Cancelled");
    });

    watcher->setFuture(future);
}

RunningJob::~RunningJob() = default;

std::shared_ptr<ResultDB> RunningJob::take() {
    return std::move(m_result);
}

void RunningJob::pause() {
    ((QFutureWatcher<SimResult>*)m_watcher)->suspend();
}
void RunningJob::resume() {
    ((QFutureWatcher<SimResult>*)m_watcher)->resume();
}
void RunningJob::cancel() {
    ((QFutureWatcher<SimResult>*)m_watcher)->cancel();
}

// =============================================================================

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

    constexpr float maxFloat = std::numeric_limits<float>::max();

    QVector3D bounds_min(maxFloat, maxFloat, maxFloat);
    QVector3D bounds_max(-maxFloat, -maxFloat, -maxFloat);

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

                bounds_min.setX(std::min(bounds_min.x(), p.x()));
                bounds_min.setY(std::min(bounds_min.y(), p.y()));
                bounds_min.setZ(std::min(bounds_min.z(), p.z()));

                bounds_max.setX(std::max(bounds_max.x(), p.x()));
                bounds_max.setY(std::max(bounds_max.y(), p.y()));
                bounds_max.setZ(std::max(bounds_max.z(), p.z()));

                ray_interaction_count += 1;
            }
        }
    }

    // Compute volume
    if (index.size() > 10) {
        auto grid_size = ceil((bounds_max - bounds_min).normalized() * 128.0);

        qDebug() << grid_size[0] << grid_size[1] << grid_size[2];

        auto cell_size = (bounds_max.x() - bounds_min.x()) / grid_size[0];

        Grid3D<float> grid(grid_size[0], grid_size[1], grid_size[2]);

        for (auto index_i = 0; index_i < index.size() - 1; index_i++) {
            auto p0 = world_to_voxel(
                verts[index[index_i]].position, bounds_min, cell_size);
            auto p1 = world_to_voxel(
                verts[index[index_i + 1]].position, bounds_min, cell_size);

            raster_segment(grid,
                           QVector3D(p0[0], p0[1], p0[2]),
                           QVector3D(p1[0], p1[1], p1[2]));
        }

        // normalize
        float largest = 0.0;
        for (auto x : grid) {
            largest = std::max(x, largest);
        }

        for (auto& x : grid) {
            x /= largest;
        }

        m_ray_volume->set_grid(
            std::move(grid), bounds_min, (bounds_max - bounds_min));
    }

    auto vertex_buffer = QByteArray(reinterpret_cast<const char*>(verts.data()),
                                    verts.size() * sizeof(LineVertex));
    auto index_buffer  = QByteArray(reinterpret_cast<const char*>(index.data()),
                                   index.size() * sizeof(uint32_t));

    qDebug() << Q_FUNC_INFO << vertex_buffer.size() << index_buffer.size()
             << bounds_min << bounds_max;

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
    setBounds(bounds_min, bounds_max);
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

// =============================================================================

void RayVolume::clean() {
    m_dirty = false;
    m_data.resize(m_grid.size() * sizeof(InstanceTableEntry));

    if (m_grid.size() == 0) { return; }

    auto delta = m_extents.x() / m_grid.size_x();

    auto dest = reinterpret_cast<InstanceTableEntry*>(m_data.data());

    for (int x = 0; x < m_grid.size_x(); x++) {
        for (int y = 0; y < m_grid.size_y(); y++) {
            for (int z = 0; z < m_grid.size_z(); z++) {
                auto p = QVector3D(x, y, z) * delta + m_origin;

                auto data = m_grid(x, y, z);

                if (data > 0.5) { qDebug() << x << y << z << data << delta; }

                float scale = delta * data;

                dest[m_grid.index(x, y, z)] =
                    calculateTableEntry(p,
                                        QVector3D(scale, scale, scale),
                                        QVector3D(),
                                        QColor::fromRgb(255, 255, 255));
            }
        }
    }
}

RayVolume::RayVolume() {
    markDirty();
    m_dirty = true;
}

void RayVolume::set_grid(Grid3D<float>&& g,
                         QVector3D       origin,
                         QVector3D       extents) {
    m_grid    = std::move(g);
    m_origin  = origin;
    m_extents = extents;
    m_dirty   = true;
    markDirty();
}

QByteArray RayVolume::getInstanceBuffer(int* instance_count) {
    if (m_dirty) clean();

    if (instance_count) { *instance_count = m_grid.size(); }

    return m_data;
}
