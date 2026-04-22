#pragma once

#include "script/script_db_interface.h"
#include "utilities/qt_helpers.h"
#include "utilities/structmodel.h"

#include <QObject>
#include <QQmlEngine>
#include <QStringList>

namespace SolTrace::GUI::Script {

// Type is integer, real, string, or vec3
struct ScriptProperty {
    QString name;
    QString type;
    QString extra;
    bool    min_bounded   = false;
    bool    max_bounded   = false;
    bool    max_inclusive = false;
    double  min           = 0.0;
    double  max           = 0.0;
    QString unit;
    QString error;

    QString value;

    RECORD_META(ScriptProperty,
                SM_EXPOSE_RO(name),
                SM_EXPOSE_RO(type),
                SM_EXPOSE_RO(extra),
                SM_EXPOSE_RO(min_bounded),
                SM_EXPOSE_RO(max_bounded),
                SM_EXPOSE_RO(max_inclusive),
                SM_EXPOSE_RO(min),
                SM_EXPOSE_RO(max),
                SM_EXPOSE_RO(unit),
                SM_EXPOSE_RO(error),
                SM_EXPOSE_RW(value));
};

class ScriptPropertyModel : public StructTableModel<ScriptProperty> {
    Q_OBJECT

public:
    explicit ScriptPropertyModel(QObject* parent = nullptr);
};

///
/// Scripts should have a header block, evaluate the script, and then try to
/// call the result with arguments
///
class Script : public QObject {
    Q_OBJECT

    QPointer<ScriptDBInterface> m_interface;
    QPointer<db::Database>      m_database;

    Q_WRITABLE_PROPERTY(QString, code, {});
    Q_READONLY_PROPERTY(QString, title);
    Q_READONLY_PROPERTY(QString, description);
    Q_READONLY_PROPERTY(bool, valid);
    Q_READONLY_PROPERTY(QStringList, parse_errors);
    Q_READONLY_PROPERTY(QStringList, run_errors);
    QOBJECT_READONLY_PROPERTY(ScriptPropertyModel, properties);

public:
    explicit Script(QObject* parent = nullptr);

    void set_database(db::Database*);

public slots:
    bool parse();

    void run();

signals:
};

} // namespace SolTrace::GUI::Script