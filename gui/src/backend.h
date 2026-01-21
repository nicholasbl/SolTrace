#pragma once

#include "database/database.h"
#include "qt_helpers.h"
#include "utilities/notification.h"

#include <QObject>
#include <QQmlEngine>
#include <QSharedPointer>

class Session : public QObject {
    Q_OBJECT

    // If data came from a file, this is the path
    Q_WRITABLE_PROPERTY(QString, current_data_path, {});

    // Current content
    std::shared_ptr<entt::registry> m_current_database;

private slots:
    void file_ready();

public:
    explicit Session(QObject* parent = nullptr);

public slots:
    void start_load_file(QUrl);

signals:

    void notification(ANotification);
};


class Backend : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit Backend(QObject* parent = nullptr);
};
