#pragma once

#include <QObject>
#include <QQuick3DGeometry>

#include "simulation_data_api.hpp"
#include "simulation_result.hpp"

namespace SD = SolTrace::Data;
namespace RD = SolTrace::Result;


using SimDataPtr = std::shared_ptr<SD::SimulationData>;

enum class RunType {
    Thread,
    Process,
};


struct ResultDB {
    using Result = RD::SimulationResult;

    Result result;

    // look up by element id. Not clean right now
    std::unordered_map<SD::element_id, std::vector<RD::ray_id>>
        element_ids_to_ray_ids;
};

/// Models a running simulation.
///
/// Provides pause and resume (if the simulation supports it)
/// Supports progress percent and text
///
/// When finished, users can collect results using the `take()` function.
/// When done (either finished or errored out), this object will destroy itself.
class RunningJob : public QObject {
    Q_OBJECT

    using Result = RD::SimulationResult;

    void* m_watcher;

    std::shared_ptr<ResultDB> m_result;

    void setup_thread(SimDataPtr data);
    void setup_process(SimDataPtr data);

public:
    explicit RunningJob(SimDataPtr data,
                        RunType    type   = RunType::Process,
                        QObject*   parent = nullptr);
    virtual ~RunningJob();

    std::shared_ptr<ResultDB> take();

public slots:
    void pause();
    void resume();

signals:
    void progress_update(int);
    void progress_text_update(QString);
    void finished();
    void error(QString);
};

// =============================================================================

// TODO make all deltas queued up for Concurrent off thread rebuilding of geom

class RayGeometry : public QQuick3DGeometry {
    Q_OBJECT
    QML_ELEMENT

    std::shared_ptr<ResultDB> m_database;

    /*
    std::unordered_set<SD::element_id> m_selected_elements;
    std::unordered_set<RD::ray_id>     m_selected_rays;
    */

    void rebuild_geometry();


public:
    using QQuick3DGeometry::QQuick3DGeometry;

    void set_database(std::shared_ptr<ResultDB>&&);

public slots:
};
