#include "backend.h"

#include "job_control/job_run_common.h"
#include "utilities/math_utility.h"

#include "surface.hpp"

#include <QFileInfo>
#include <QFuture>
#include <QFutureWatcher>
#include <QStringView>
#include <QTextStream>
#include <QtConcurrentRun>

// TODO COORDINATE SYSTEMS. the sim could be arbitrary


Session::Session(QObject* parent) : QObject(parent) { }

struct LoadedFile {
    QString    name;
    QString    provenance;
    SimDataPtr ptr;
};

using LoadResult = std::variant<LoadedFile, ANotification>;


static LoadResult load_file(QString fname) {
    qDebug() << Q_FUNC_INFO << fname;

    auto file = QFileInfo(fname);

    if (!(file.isFile() && file.isReadable())) {
        return ANotification::error("Unable to open file for reading");
    }

    auto new_data = std::make_shared<SD::SimulationData>();

    // convert to normal char string
    auto str = fname.toStdString();

    if (!new_data->import_from_file(str)) {
        return ANotification::error("Unable to import file");
    }

    return LoadedFile {
        .name       = file.completeBaseName(),
        .provenance = fname,
        .ptr        = new_data,
    };
}

void Session::file_ready() {
    auto from = dynamic_cast<QFutureWatcher<LoadResult>*>(sender());

    if (!from) { qFatal("this shouldn't happen"); }

    if (from->isCanceled()) {
        qInfo() << "File load cancelled";
        return;
    }

    auto result = from->result();

    std::visit(overloaded {
                   [this](LoadedFile arg) {
                       // convert

                       this->set_current_data_path(arg.provenance);
                       m_current_database = db::import(*arg.ptr);
                   },
                   [this](ANotification arg) { emit notification(arg); },
               },
               result);
}


void Session::start_load_file(QUrl file) {
    qDebug() << Q_FUNC_INFO << file;

    auto watcher = new QFutureWatcher<LoadResult>();

    connect(watcher,
            &QFutureWatcher<LoadResult>::finished,
            this,
            &Session::file_ready);

    connect(watcher,
            &QFutureWatcher<LoadResult>::finished,
            watcher,
            &QObject::deleteLater);

    auto future = QtConcurrent::run(load_file, file.toLocalFile());

    watcher->setFuture(future);
}


#if 0

bool DataSetsModel::_can_append_new(QVariant const&) {
    return true;
}

void DataSetsModel::_append_new(QVariant) {
    auto new_data    = std::make_shared<SD::SimulationData>();
    auto new_wrapper = std::make_shared<LocalData>(new_data);

    watch(new_wrapper.get());

    // need a better naming scheme

    auto new_set = ADataSet {
        .name = QString("Untitled %1").arg(m_sets.size()),
        .ptr  = new_wrapper,
    };

    auto at = _record_count();

    beginInsertRows(QModelIndex(), at, at);
    m_sets.push_back(new_set);
    endInsertRows();

    if (!current_data()) {
        select(0);
    }
}

bool DataSetsModel::_can_delete_at(size_t index, size_t count) {
    return true;
}

void DataSetsModel::_delete_at(size_t index, size_t count) {
    m_sets.remove(index, count);
}

int DataSetsModel::_record_count() const {
    return m_sets.size();
}

void DataSetsModel::_clear() {
    m_sets.clear();
}

void DataSetsModel::watch(LocalData* ptr) {
    // connect(ptr, &Data::modified_changed, this,
    // &DataSetsModel::a_data_changed);
}

DataSetsModel::DataSetsModel(QObject* parent) : IndirectTableModel(parent) {
    add_properties({
        {
            .display_name = "name",
            .getter       = [this](auto index) -> QVariant {
                return this->m_sets.value(index).name;
            },
        },
        {
            .display_name = "provenance",
            .getter       = [this](auto index) -> QVariant {
                return this->m_sets.value(index).provenance;
            },
        },
    });
}



void DataSetsModel::file_ready() {
    auto from = dynamic_cast<QFutureWatcher<LoadResult> *>(sender());

    if (!from) { qFatal("this shouldnt happen"); }

    if (from->isCanceled()) {
        qInfo() << "File load cancelled";
        return;
    }

    auto result = from->result();

    std::visit(overloaded{
                   [this](LoadedFile arg) {
                       // convert

                       auto new_set = ADataSet {
                           .name       = arg.name,
                           .provenance = arg.provenance,
                           .ptr        = std::make_shared<LocalData>(arg.ptr),
                       };

                       auto at = _record_count();

                       watch(new_set.ptr.get());

                       beginInsertRows(QModelIndex(), at, at);
                       m_sets.push_back(new_set);
                       endInsertRows();

                       if (!current_data()) {
                           select(0);
                       }
                   },
                   [this](QString arg) {
                       emit file_load_error(
                           QString("Unable to load file: %1").arg(arg));
                   },
               },
               result);
}

void DataSetsModel::a_data_changed() {
    auto who = qobject_cast<LocalData*>(sender());

    if (!who) return;

    for (int i = 0; i < m_sets.size(); i++) {

        auto const& c = m_sets[i];

        if (c.ptr.get() == who) {
            this->notify_update(i);
            return;
        }
    }
}


void DataSetsModel::select(int index) {
    qDebug() << Q_FUNC_INFO << index;
    if (index < 0 or index >= m_sets.size()) return;

    set_current_data(m_sets[index].ptr.get());
}
#endif

// =============================================================================

Backend::Backend(QObject* parent) : QObject { parent } { }
