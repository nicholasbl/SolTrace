#pragma once

#include "analysis/ray_geometry.h"
#include "database/database.h"
#include "database/database_models.h"
#include "database/worldgeometrymodel.h"
#include "job_control/job_run.h"
#include "qt_helpers.h"
#include "utilities/notification.h"

#include <QObject>
#include <QQmlEngine>
#include <QSharedPointer>


class JobBackend : public QObject {
    Q_OBJECT

    // Current content
    QPointer<db::Database> m_current_database;

    QPointer<RunningJob> m_running;

    enum class State {
        IDLE,
        RUNNING,
    };

    Q_ENUM(State);

    Q_WRITABLE_PROPERTY(State, state, State::IDLE);
    Q_WRITABLE_PROPERTY(int, progress, 0);
    Q_WRITABLE_PROPERTY(QString, job_log, {});

public:
    explicit JobBackend(QObject* parent = nullptr);
    virtual ~JobBackend() = default;

    void install(db::Database*);

private slots:
    void job_done();

public slots:
    void start();
    void stop();

signals:
    void notify(ANotification);

    void new_results(std::shared_ptr<ResultDB>);
};

//==============================================================================

class ResultsBackend : public QObject {
    Q_OBJECT

    std::shared_ptr<ResultDB> m_results;

    std::unique_ptr<analysis::RayGeometry> m_ray_geometry;

    Q_PROPERTY(analysis::RayGeometry* ray_geometry READ ray_geometry NOTIFY
                   ray_geometry_changed FINAL)

public:
    explicit ResultsBackend(QObject* parent = nullptr);
    virtual ~ResultsBackend() = default;

    analysis::RayGeometry* ray_geometry() const;

public slots:
    void set_results(std::shared_ptr<ResultDB>);

signals:
    void ray_geometry_changed();
};

//==============================================================================

class Backend : public QObject {
    Q_OBJECT

    QML_ELEMENT
    QML_SINGLETON

    // If data came from a file, this is the path
    Q_WRITABLE_PROPERTY(QString, current_data_path, {});

    // Current content
    QPointer<db::Database> m_current_database;

    QOBJECT_READONLY_PROPERTY(JobBackend, job_backend);
    QOBJECT_READONLY_PROPERTY(ResultsBackend, results_backend);

    QOBJECT_READONLY_PROPERTY(db::BreadcrumbModel, breadcrumb_model);
    QOBJECT_READONLY_PROPERTY(db::ChildModel, child_model);
    QOBJECT_READONLY_PROPERTY(db::RenderGroupsModel, render_groups_model);
    QOBJECT_READONLY_PROPERTY(db::TagsModel, tags_model);
    QOBJECT_READONLY_PROPERTY(db::AnInstanceEditor, instance_edit_model);
    QOBJECT_READONLY_PROPERTY(db::WorldGeometryModel, world_geometry_model);


    void install(db::Database*);

private slots:
    void file_ready();

public:
    explicit Backend(QObject* parent = nullptr);

public slots:
    void reset();
    void start_load_file(QUrl);

signals:
    void notification(ANotification);
};
