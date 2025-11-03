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
                    if (element >= 0) {
                        ret->element_ids_to_ray_ids[interaction->element]
                            .push_back((*iter)->id);
                    }
                }
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
        clear();
        return;
    }

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

    {

        for (auto iter = m_database->result.get_ray_record_iteratior();
             !m_database->result.is_at_end(iter);
             ++iter) {

            auto& this_ray = (**iter);

            for (size_t interaction_i = 0;
                 interaction_i < this_ray.interactions.size();
                 interaction_i++) {

                // install vert
                auto const& interaction = this_ray.interactions[interaction_i];

                auto p = convert(interaction->location.data);

                float percent = (float)interaction_i /
                                ((float)this_ray.interactions.size() - 1);

                verts.push_back({ .position = p, .uv = QVector2D(percent, 0) });


                // install index

                index.push_back(verts.size() - 1);
                index.push_back(verts.size());

                bounds_min.setX(std::min(bounds_min.x(), p.x()));
                bounds_min.setY(std::min(bounds_min.y(), p.y()));
                bounds_min.setZ(std::min(bounds_min.z(), p.z()));

                bounds_max.setX(std::max(bounds_max.x(), p.x()));
                bounds_max.setY(std::max(bounds_max.y(), p.y()));
                bounds_max.setZ(std::max(bounds_max.z(), p.z()));
            }
        }
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

void RayGeometry::set_database(std::shared_ptr<ResultDB>&& data) {
    qDebug() << "New ray geometry database";
    m_database = std::move(data);
    rebuild_geometry();
}
