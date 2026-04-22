#pragma once

#include "database/database.h"
#include "utilities/notification.h"
#include "utilities/qt_helpers.h"

#include <QObject>
#include <QUrl>

namespace SolTrace::GUI::App {

class FileSourceModule : public QObject {
    Q_OBJECT
    QML_ELEMENT

    QOBJECT_WRITABLE_PROPERTY(db::Database, current_database)

    Q_WRITABLE_PROPERTY(bool, is_loading, false)

private slots:
    void file_ready();

public:
    FileSourceModule(QObject* parent = nullptr);
    Q_WRITABLE_PROPERTY(QString, name, "Untitled")

public slots:
    void load_url(QUrl);
    void load_new();

signals:
    void notify(ANotification);
    void cancel_current_load(QPrivateSignal);
};

} // namespace SolTrace::GUI::App
