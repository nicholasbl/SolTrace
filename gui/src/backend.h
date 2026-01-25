#pragma once

#include "database/database.h"
#include "qt_helpers.h"
#include "utilities/notification.h"

#include <QObject>
#include <QQmlEngine>
#include <QSharedPointer>

class Session : public QObject {
    Q_OBJECT

    QML_ELEMENT
    QML_SINGLETON

    // If data came from a file, this is the path
    Q_WRITABLE_PROPERTY(QString, current_data_path, {});

    // Current content
    QPointer<db::Database> m_current_database;

private slots:
    void file_ready();

public:
    explicit Session(QObject* parent = nullptr);

public slots:
    void reset();
    void start_load_file(QUrl);

signals:

    void notification(ANotification);
};
