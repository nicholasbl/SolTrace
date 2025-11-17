#include "job_run.h"

#include "dataset.h"
#include "native_runner/native_runner.hpp"
#include "simulation_result.hpp"
#include "simulation_runner.hpp"
#include "utility.h"

#include <QException>
#include <QFutureWatcher>
#include <QGuiApplication>
#include <QPromise>
#include <QtConcurrentRun>

// Native only for the moment

using ResultPtr = std::shared_ptr<ResultDB>;
using SimResult = std::variant<ResultPtr, QString>;

// Qconcurrent will auto call start and finish on the promise

#define SOLTRACE_SECTION(FUNC, VALUE, TEXT)                                    \
    promise.setProgressValueAndText(VALUE, TEXT);                              \
    promise.suspendIfRequested();                                              \
    if (promise.isCanceled()) { return; }                                      \
    result = current_runner->FUNC;                                             \
    if (result == SolTrace::Runner::RunnerStatus::ERROR) {                     \
        promise.emplaceResult(QString(TEXT " failed"));                        \
        return;                                                                \
    }

#define SECTION(VALUE, TEXT)                                                   \
    promise.setProgressValueAndText(VALUE, TEXT);                              \
    promise.suspendIfRequested();                                              \
    if (promise.isCanceled()) { return; }

static void execute_runner(QPromise<SimResult>& promise, SimDataPtr data) {
    try {
        promise.setProgressRange(0, 100);

        auto current_runner =
            std::make_unique<SolTrace::NativeRunner::NativeRunner>();

        SolTrace::Runner::RunnerStatus result;

        SOLTRACE_SECTION(initialize(), 0, "Starting simulation");

        SOLTRACE_SECTION(setup_simulation(data.get()), 0, "Setup simulation");

        SOLTRACE_SECTION(run_simulation(), 10, "Run simulation");

        // maybe map simulation work to 10 - 90 %?

        auto ret = std::make_shared<ResultDB>();

        SOLTRACE_SECTION(
            report_simulation(&(ret->result), 100), 90, "Report simulation");

        // now compute lookup tables

        SECTION(90, "Building lookup tables");

        auto& st_result = ret->result;

        auto num_records = st_result.get_number_of_records();

        if (num_records > 0) {
            ret->element_ids_to_ray_ids.reserve(num_records);

            uint64_t iter_count = 0;

            for (auto iter = st_result.get_ray_record_iteratior();
                 !st_result.is_at_end(iter);
                 ++iter) {
                auto progress =
                    lerp<float, float>(iter_count, 0, num_records - 1, 90, 100);

                SECTION(progress, "Building lookup tables");

                for (auto const& interaction : (*iter)->interactions) {
                    auto element = interaction->element;
                    // It looks like invalid element IDs are negative
                    // Any zero+ element id could be used
                    if (element >= 0) {
                        ret->element_ids_to_ray_ids[interaction->element]
                            .push_back((*iter)->id);
                    }
                }

                iter_count++;
            }
        }


        SECTION(100, "Done");

        promise.emplaceResult(std::move(ret));

    } catch (std::exception& e) {
        promise.emplaceResult(QString(e.what()));
        return;
    }
}

void RunningJob::setup_thread(SimDataPtr data) {
    auto future = QtConcurrent::run(execute_runner, data);

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
void RunningJob::setup_process(SimDataPtr data) {
#ifdef Q_OS_WASM
    return setup_thread(data);
#else
    {
        // dump to a temp file or memory

        // launch another process using this
        // qApp->applicationFilePath();

        // have it read the stuff and return

        return setup_thread(data);
    }
#endif
}

RunningJob::RunningJob(SimDataPtr data, RunType type, QObject* parent)
    : QObject(parent) {
    switch (type) {
    case RunType::Thread: setup_thread(data); break;
    case RunType::Process: setup_process(data); break;
    }
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

// =============================================================================

struct LineVertex {
    QVector3D position;
    QVector2D uv;
};

void RayGeometry::rebuild_geometry() {
    if (!m_database) {
        return;
    }

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

    // for (auto l : verts) {
    //     qDebug() << "V" << l.position << l.uv;
    // }

    // for (auto l : index) {
    //     qDebug() << "I" << l;
    // }

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
