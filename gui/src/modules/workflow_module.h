#pragma once

#include <QObject>
#include "utilities/qt_helpers.h"
#include "backend.h"
#include "module_common.h"

namespace SolTrace::GUI::App {

/**
 * @class WorkflowModule
 * @brief Models the application workflow structure for QML navigation.
 *
 * Encodes the three-level hierarchy: Phase → Section → Module
 *
 * - Phase:   Configuration, Simulation, Analysis
 * - Section: Sub-groupings within each phase
 *
 * The phase and section properties drive which panel is displayed in the UI.
 * The static get_*_sections() methods provide display strings for tab
 * rendering.
 *
 * Note: Phase and section enums are currently hardcoded.
 */
class WorkflowModule : public QObject {
    Q_OBJECT

public:
    explicit WorkflowModule(QObject* parent = nullptr);

    enum class Phase { Configuration, Simulation, Analysis };
    enum class ConfigurationSection { Sun, Tracing, Materials, Geometry };
    enum class SimulationSection { Execution, Navigation, Diagnostics };
    enum class AnalysisSection { Intersections, Flux };

    Q_ENUM(Phase)
    Q_ENUM(ConfigurationSection)
    Q_ENUM(SimulationSection)
    Q_ENUM(AnalysisSection)

    Q_WRITABLE_PROPERTY(Phase, phase, Phase::Configuration)
    Q_WRITABLE_PROPERTY(ConfigurationSection,
                        configuration_section,
                        ConfigurationSection::Sun);
    Q_WRITABLE_PROPERTY(SimulationSection,
                        simulation_section,
                        SimulationSection::Execution);
    Q_WRITABLE_PROPERTY(AnalysisSection,
                        analysis_section,
                        AnalysisSection::Intersections);

    Q_INVOKABLE static QStringList phases();
    Q_INVOKABLE static QStringList configuration_sections();
    Q_INVOKABLE static QStringList simulation_sections();
    Q_INVOKABLE static QStringList analysis_sections();
};


} // namespace SolTrace::GUI::App

