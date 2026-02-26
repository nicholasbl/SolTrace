#include "workflow_module.h"
#include <QMetaEnum>

namespace SolTrace::GUI::App {

WorkflowModule::WorkflowModule(QObject *parent) :
      QObject(parent)
{}

QStringList WorkflowModule::phases()
{
    static QStringList keys;
    if (keys.isEmpty()) {
        QMetaEnum meta = QMetaEnum::fromType<Phase>();
        for (int i = 0; i < meta.keyCount(); i++) {
            keys.append(meta.key(i));
        }
    }
    return keys;
}

QStringList WorkflowModule::configuration_sections()
{
    static QStringList keys;
    if (keys.isEmpty()) {
        QMetaEnum meta = QMetaEnum::fromType<ConfigurationSection>();
        for (int i = 0; i < meta.keyCount(); i++) {
            keys.append(meta.key(i));
        }
    }
    return keys;
}

QStringList WorkflowModule::simulation_sections()
{
    static QStringList keys;
    if (keys.isEmpty()) {
        QMetaEnum meta = QMetaEnum::fromType<SimulationSection>();
        for (int i = 0; i < meta.keyCount(); i++) {
            keys.append(meta.key(i));
        }
    }
    return keys;
}

QStringList WorkflowModule::analysis_sections()
{
    static QStringList keys;
    if (keys.isEmpty()) {
        QMetaEnum meta = QMetaEnum::fromType<AnalysisSection>();
        for (int i = 0; i < meta.keyCount(); i++) {
            keys.append(meta.key(i));
        }
    }
    return keys;
}

} // namespace SolTrace::GUI::App
