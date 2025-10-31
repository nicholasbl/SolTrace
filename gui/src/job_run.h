#pragma once

#include <QObject>

#include "dataset.h"

#include "simulation_result.hpp"

enum class RunType {
    Thread,
    Process,
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

    using Result = SolTrace::Result::SimulationResult;

    void* m_watcher;

    std::unique_ptr<Result> m_result;

    void setup_thread(SimDataPtr data);
    void setup_process(SimDataPtr data);

public:
    explicit RunningJob(SimDataPtr data,
                        RunType    type   = RunType::Process,
                        QObject*   parent = nullptr);
    virtual ~RunningJob();

    std::unique_ptr<Result> take();

public slots:
    void pause();
    void resume();

signals:
    void progress_update(int);
    void progress_text_update(QString);
    void finished();
    void error(QString);
};
