#pragma once

#include <QObject>
#include "utilities/qt_helpers.h"
#include "backend.h"
#include "module_common.h"

namespace SolTrace::GUI::App {

/**
 * @class FluxModule
 * @brief Flux analysis module.
 *
 * Provides access to flux distribution results from the simulation.
 * Shares ResultsBackend with Intersections — both modules read from
 * the same simulation result data.
 *
 * QML access pattern: App.flux.results
 */
class FluxModule : public QObject {
    Q_OBJECT

public:
    explicit FluxModule(QObject* parent = nullptr);

    QPOINTER_WRITABLE_PROPERTY(ResultsBackend, results)
    QPOINTER_WRITABLE_PROPERTY(FluxBackend, backend)
};

} // namespace SolTrace::GUI::App
