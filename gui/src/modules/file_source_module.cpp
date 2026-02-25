#include "file_source_module.h"
#include "utilities/math_utility.h"

#include <QtConcurrent/qtconcurrentrun.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qfuturewatcher.h>

namespace SolTrace::GUI::App {


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

using ResultFuture = QFutureWatcher<LoadResult>;


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


void FileSource::file_ready() {
    qDebug() << Q_FUNC_INFO;
    auto from = dynamic_cast<ResultFuture*>(sender());

    if (!from) { qFatal("this shouldn't happen"); }

    if (from->isCanceled()) {
        emit notify(ANotification::info("File load cancelled"));
        qInfo() << "File load cancelled";
        return;
    }

    auto result = from->result();

    std::visit(
        overloaded {
            [this](LoadedFile arg) { this->set_current_database(arg.ptr); },
            [this](LoadFileFailed failure) {
                emit this->notify(failure.notification);
                delete failure.ptr;
            },
        },
        result);
}

void FileSource::handle_source_update() {
    if (is_loading()) { emit cancel_current_load(QPrivateSignal {}); }
    set_is_loading(false);

    auto new_source = source();

    if (new_source.isEmpty()) {
        qDebug() << Q_FUNC_INFO;
        set_current_database(new db::Database(this));
        return;
    }


    qDebug() << Q_FUNC_INFO << new_source;

    auto watcher = new ResultFuture(this);

    connect(
        this, &FileSource::cancel_current_load, watcher, &ResultFuture::cancel);

    connect(watcher, &ResultFuture::finished, this, &FileSource::file_ready);

    connect(watcher, &ResultFuture::finished, watcher, &QObject::deleteLater);

    auto ptr = new db::Database(this);

    auto future = QtConcurrent::run(load_file, new_source.toLocalFile(), ptr);

    watcher->setFuture(future);
}

FileSource::FileSource(QObject* parent) : QObject { parent } {
    connect(this,
            &FileSource::source_changed,
            this,
            &FileSource::handle_source_update);
}


} // namespace SolTrace::GUI::App
