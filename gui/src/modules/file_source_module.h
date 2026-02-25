#pragma once

#include "database/database.h"
#include "utilities/notification.h"
#include "utilities/qt_helpers.h"

#include <QObject>
#include <QUrl>

namespace SolTrace::GUI::App {

class FileSource : public QObject {
    Q_OBJECT

    QOBJECT_WRITABLE_PROPERTY(db::Database, current_database)

    Q_WRITABLE_PROPERTY(bool, is_loading, false)
    Q_WRITABLE_PROPERTY(QUrl, source, {})

private slots:
    void file_ready();
    void handle_source_update();

signals:
    void cancel_current_load(QPrivateSignal);

public:
    FileSource(QObject* parent = nullptr);

signals:
    void notify(ANotification);
};

} // namespace SolTrace::GUI::App
