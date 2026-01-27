#pragma once

#include "database/database.h"
#include "database/database_models.h"
#include "qt_helpers.h"
#include "utilities/notification.h"

#include <QObject>
#include <QQmlEngine>
#include <QSharedPointer>


class Backend : public QObject {
    Q_OBJECT

    QML_ELEMENT
    QML_SINGLETON

    // If data came from a file, this is the path
    Q_WRITABLE_PROPERTY(QString, current_data_path, {});

    // Current content
    QPointer<db::Database> m_current_database;

    QOBJECT_READONLY_PROPERTY(db::BreadcrumbModel, breadcrumb_model);
    QOBJECT_READONLY_PROPERTY(db::ChildModel, child_model);
    QOBJECT_READONLY_PROPERTY(db::GroupsModel, groups_model);
    QOBJECT_READONLY_PROPERTY(db::TagsModel, tags_model);
    QOBJECT_READONLY_PROPERTY(db::AnInstanceEditor, instance_edit_model);

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
