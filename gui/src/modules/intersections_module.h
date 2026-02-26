#pragma once

#include <QObject>
#include "utilities/qt_helpers.h"
#include "backend.h"
#include "module_common.h"

namespace SolTrace::GUI::App {

/**
 * @class IntersectionsModule
 * @brief Ray intersection analysis module.
 *
 * Provides access to intersection results from the simulation.
 * Holds non-owning references to both the shared results backend
 * and the intersections-specific backend.
 *
 * QML access pattern: App.intersections.results
 */
class IntersectionsModule : public QObject {
    Q_OBJECT

public:
    explicit IntersectionsModule(QObject* parent = nullptr);

    QPOINTER_WRITABLE_PROPERTY(ResultsBackend, results)
    QPOINTER_WRITABLE_PROPERTY(IntersectionsBackend, backend)
};


} // namespace SolTrace::GUI::App
