#pragma once

#include "database/database.h"
#include "qt_helpers.h"

#include <QObject>
#include <QQmlEngine>
#include <QSharedPointer>

// struct ADataSet {
//     QString name;
//     QString provenance;
//     DataPtr ptr;
// };

// class DataSetsModel : public IndirectTableModel {
//     Q_OBJECT
//     QVector<ADataSet> m_sets;

//     bool _can_append_new(QVariant const&) override;
//     void _append_new(QVariant) override;
//     bool _can_delete_at(size_t, size_t) override;
//     void _delete_at(size_t, size_t) override;
//     int  _record_count() const override;
//     void _clear() override;

//     void watch(LocalData* ptr);

//     QOBJECT_WRITABLE_PROPERTY(LocalData, current_data);

// private slots:
//     void file_ready();
//     void a_data_changed();


// public:
//     explicit DataSetsModel(QObject* parent = nullptr);

// public slots:
//     void start_load_file(QUrl);

//     void select(int);

// signals:
//     void file_load_error(QString);
// };


class Backend : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    std::shared_ptr<entt::registry> m_current_database;

    Q_WRITABLE_PROPERTY(QString, current_data_path, {});

public:
    explicit Backend(QObject* parent = nullptr);
};
