#include "session.h"


namespace SolTrace::GUI {

    SessionManager::SessionManager(QObject* parent) {}


    QStringList WorkflowModel::get_workflow_phases() {
        return enum_to_stringlist<WorkflowPhase>();
    }

    QStringList WorkflowModel::get_configuration_sections() {
        return enum_to_stringlist<ConfigurationSection>();
    }
    QStringList WorkflowModel::get_simulation_sections() {
        return enum_to_stringlist<SimulationSection>();
    }
    QStringList WorkflowModel::get_analysis_sections() {
        return enum_to_stringlist<AnalysisSection>();
    }
}
