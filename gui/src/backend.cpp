#include "backend.h"

#include <QFileInfo>
#include <QFuture>
#include <QFutureWatcher>
#include <QStringView>
#include <QTextStream>
#include <QtConcurrentRun>

template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};


bool DataSetsModel::_can_append_new(QVariant const&) {
    return true;
}

void DataSetsModel::_append_new(QVariant) {
    auto new_data    = std::make_shared<SD::SimulationData>();
    auto new_wrapper = std::make_shared<Data>(new_data);

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

    if (!current_data()) { select(0); }
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

void DataSetsModel::watch(Data* ptr) {
    connect(ptr, &Data::modified_changed, this, &DataSetsModel::a_data_changed);
}

DataSetsModel::DataSetsModel(QObject* parent) : IndirectTableModel(parent) {
    add_property({
        .display_name = "name",
        .getter       = [this](auto index) -> QVariant {
            return this->m_sets.value(index).name;
        },
    });

    add_property({
        .display_name = "provenance",
        .getter       = [this](auto index) -> QVariant {
            return this->m_sets.value(index).provenance;
        },
    });
}

struct LoadedFile {
    QString    name;
    QString    provenance;
    SimDataPtr ptr;
};

using LoadResult = std::variant<LoadedFile, QString>;

static LoadResult load_file(QString fname) {
    qDebug() << Q_FUNC_INFO << fname;

    auto file = QFileInfo(fname);

    if (!(file.isFile() && file.isReadable())) {
        return QStringLiteral("Unable to open file for reading");
    }

    auto new_data = std::make_shared<SD::SimulationData>();

    // convert to normal char string
    auto str = fname.toStdString();

    if (!new_data->import_from_file(str)) {
        return QStringLiteral("Unable to import file");
    }


    return LoadedFile {
        .name       = file.completeBaseName(),
        .provenance = fname,
        .ptr        = new_data,
    };
}


void DataSetsModel::file_ready() {
    auto from = dynamic_cast<QFutureWatcher<LoadResult>*>(sender());

    if (!from) { qFatal("this shouldnt happen"); }

    if (from->isCanceled()) {
        qInfo() << "File load cancelled";
        return;
    }

    auto result = from->result();

    std::visit(overloaded {
                   [this](LoadedFile arg) {
                       // convert

                       auto new_set = ADataSet {
                           .name       = arg.name,
                           .provenance = arg.provenance,
                           .ptr        = std::make_shared<Data>(arg.ptr),
                       };

                       auto at = _record_count();

                       watch(new_set.ptr.get());

                       beginInsertRows(QModelIndex(), at, at);
                       m_sets.push_back(new_set);
                       endInsertRows();

                       if (!current_data()) { select(0); }
                   },
                   [this](QString arg) {
                       emit file_load_error(
                           QString("Unable to load file: %1").arg(arg));
                   },
               },
               result);
}

void DataSetsModel::a_data_changed() {
    auto who = qobject_cast<Data*>(sender());

    if (!who) return;

    for (int i = 0; i < m_sets.size(); i++) {

        auto const& c = m_sets[i];

        if (c.ptr.get() == who) {
            this->notify_update(i);
            return;
        }
    }
}

void DataSetsModel::start_load_file(QUrl file) {
    qDebug() << Q_FUNC_INFO << file;

    auto watcher = new QFutureWatcher<LoadResult>();

    connect(watcher,
            &QFutureWatcher<LoadResult>::finished,
            this,
            &DataSetsModel::file_ready);

    connect(watcher,
            &QFutureWatcher<LoadResult>::finished,
            watcher,
            &QObject::deleteLater);

    auto future = QtConcurrent::run(load_file, file.toLocalFile());

    watcher->setFuture(future);
}

void DataSetsModel::select(int index) {
    qDebug() << Q_FUNC_INFO << index;
    if (index < 0 or index >= m_sets.size()) return;

    set_current_data(m_sets[index].ptr.get());
}

// =============================================================================

Backend::Backend(QObject* parent)
    : QObject { parent }, m_data_sets(new DataSetsModel(this)) { }
