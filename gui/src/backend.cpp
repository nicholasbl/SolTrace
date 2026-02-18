#include "backend.h"

#include "job_control/job_run_common.h"
#include "utilities/math_utility.h"

#include "surface.hpp"

#include <QApplication>
#include <QFileInfo>
#include <QFuture>
#include <QFutureWatcher>
#include <QStringView>
#include <QTextStream>
#include <QtConcurrentRun>

// TODO COORDINATE SYSTEMS. the sim could be arbitrary

JobBackend::JobBackend(QObject* parent) { }

void JobBackend::install(db::Database* db) {
    m_current_database = db;
}

void JobBackend::job_done() {
    qDebug() << Q_FUNC_INFO;

    auto* from = qobject_cast<RunningJob*>(sender());
    if (!from) {
        qCritical() << Q_FUNC_INFO << "bad cast";
        return;
    }

    if (m_running != from) {
        qCritical() << Q_FUNC_INFO << m_running << from;
        return;
    }

    set_state(State::IDLE);

    m_running = nullptr;

    auto results = from->take();

    emit new_results(results);
}

void JobBackend::start() {
    qDebug() << Q_FUNC_INFO;
    if (!m_current_database) return;

    if (m_running) {
        emit notify(
            ANotification::error("A running job is already in progress."));
        return;
    }

    qDebug() << Q_FUNC_INFO << "Launch";
    m_running = new RunningJob(
        m_current_database->export_to_simdata(), RunType::Thread, this);

    connect(m_running,
            &RunningJob::progress_update,
            this,
            &JobBackend::set_progress);
    connect(m_running,
            &RunningJob::progress_text_update,
            this,
            &JobBackend::set_job_log);

    connect(m_running, &RunningJob::finished, this, &JobBackend::job_done);
    connect(m_running, &RunningJob::finished, this, &RunningJob::deleteLater);
    connect(this, &QObject::destroyed, m_running, &RunningJob::cancel);

    set_state(State::RUNNING);
}
void JobBackend::stop() { }

//==============================================================================

ResultsBackend::ResultsBackend(QObject* parent)
    : QObject(parent), m_ray_geometry(new analysis::RayGeometry) { }

void ResultsBackend::set_results(std::shared_ptr<ResultDB> ptr) {
    m_ray_geometry->set_results(ptr);
}

analysis::RayGeometry* ResultsBackend::ray_geometry() const {
    return m_ray_geometry.get();
}

//==============================================================================

Backend::Backend(QObject* parent)
    : QObject(parent),
      m_job_backend(new JobBackend(this)),
      m_results_backend(new ResultsBackend(this)),
      m_breadcrumb_model(new db::BreadcrumbModel(this)),
      m_child_model(new db::ChildModel(this)),
      m_render_groups_model(new db::RenderGroupsModel(this)),
      m_tags_model(new db::TagsModel(this)),
      m_instance_edit_model(new db::AnInstanceEditor(this)),
      m_world_geometry_model(new db::WorldGeometryModel(this)) {
    connect(m_job_backend, &JobBackend::notify, this, &Backend::notification);

    connect(m_job_backend,
            &JobBackend::new_results,
            m_results_backend,
            &ResultsBackend::set_results);
}

struct LoadedFile {
    QString       name;
    QString       provenance;
    db::Database* ptr;
};

struct LoadFileFailed {
    ANotification notification;
    db::Database* ptr;

    LoadFileFailed(QString message, db::Database* db)
        : notification(ANotification::error(message)), ptr(db) { }
};

using LoadResult = std::variant<LoadedFile, LoadFileFailed>;


static void load_file(QPromise<LoadResult>& result,
                      QString               fname,
                      db::Database*         destination) {
    result.setProgressRange(0, 100);
    qDebug() << Q_FUNC_INFO << fname;

    result.setProgressValueAndText(0, "Reading file...");

    auto file = QFileInfo(fname);

    if (!(file.isFile() && file.isReadable())) {
        result.emplaceResult(
            LoadFileFailed("Unable to open file for reading", destination));
        return;
    }

    auto new_data = std::make_shared<SD::SimulationData>();

    auto str = fname.toStdString();

    if (!new_data->import_from_file(str)) {
        result.emplaceResult(
            LoadFileFailed("Unable to import file", destination));
        return;
    }

    result.setProgressValueAndText(50, "Importing content...");

    destination->import(*new_data);

    result.setProgressValueAndText(100, "Done");

    result.emplaceResult(LoadedFile {
        .name       = file.completeBaseName(),
        .provenance = fname,
        .ptr        = destination,
    });
}

using ResultFuture = QFutureWatcher<LoadResult>;

void Backend::install(db::Database* db) {
    qDebug() << Q_FUNC_INFO << db;

    if (m_current_database) { delete m_current_database; }

    m_current_database = db;

    if (db) { db->setParent(this); }

    m_job_backend->install(db);

    m_breadcrumb_model->reset(db);
    m_child_model->reset(db);
    m_render_groups_model->reset(db);
    m_tags_model->reset(db);
    m_instance_edit_model->reset(db);
    m_world_geometry_model->reset(db);
}

void Backend::file_ready() {
    qDebug() << Q_FUNC_INFO;
    auto from = dynamic_cast<ResultFuture*>(sender());

    if (!from) { qFatal("this shouldn't happen"); }

    if (from->isCanceled()) {
        emit notification(ANotification::info("File load cancelled"));
        qInfo() << "File load cancelled";
        return;
    }

    auto result = from->result();

    std::visit(overloaded {
                   [this](LoadedFile arg) {
                       this->set_current_data_path(arg.provenance);

                       this->install(arg.ptr);
                   },
                   [this](LoadFileFailed failure) {
                       emit this->notification(failure.notification);
                       delete failure.ptr;
                   },
               },
               result);
}

void Backend::reset() {
    qDebug() << Q_FUNC_INFO;
    this->install(new db::Database());
}

void Backend::start_load_file(QUrl file) {
    qDebug() << Q_FUNC_INFO << file;

    auto watcher = new ResultFuture(this);

    connect(watcher, &ResultFuture::finished, this, &Backend::file_ready);

    connect(watcher, &ResultFuture::finished, watcher, &QObject::deleteLater);

    auto ptr = new db::Database(this);

    auto future = QtConcurrent::run(load_file, file.toLocalFile(), ptr);

    watcher->setFuture(future);
}
