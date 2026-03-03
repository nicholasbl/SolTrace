#pragma once

#include <QObject>
#include "utilities/qt_helpers.h"
#include "backend.h"
#include "module_common.h"

namespace SolTrace::GUI::App {

/**
 * @class TracingModule
 * @brief Ray tracing engine configuration module.
 *
 * Configures the ray tracing engine used for simulation execution.
 * The active tracer selection determines which backend implementation is used:
 *
 * - Legacy:     Original SolTrace tracer
 * - Refactored: Modernized CPU tracer
 * - GPU:        GPU-accelerated tracer (default)
 *
 * QML access pattern: App.tracing.max_intersections
 */
class TracingModule : public QObject {
    Q_OBJECT

public:
    explicit TracingModule(QObject* parent = nullptr);

    QOBJECT_WRITABLE_PROPERTY(TracingBackend, backend)
    QOBJECT_READONLY_PROPERTY(StatusComponent, status)

    enum class Tracer { Legacy, Refactored, GPU };

    Q_ENUM(Tracer)

    Q_WRITABLE_PROPERTY(Tracer, tracer, Tracer::GPU)

   // ─── Engine Properties
   // ───────────────────────────────────────────────────────────────────

    Q_WRITABLE_PROPERTY(double, max_intersections, 3.5e6)
    Q_WRITABLE_PROPERTY(double, max_rays, 1e6)
    Q_WRITABLE_PROPERTY(bool, sun_shape, false)

   // ─── Execution Properties
   // ───────────────────────────────────────────────────────────────────

    Q_WRITABLE_PROPERTY(int, cpu_cores, 4)
    Q_WRITABLE_PROPERTY(int, seed_value, 12345)

   // ─── Optimization Properties
   // ───────────────────────────────────────────────────────────────────

    Q_WRITABLE_PROPERTY(bool, optical_errors, false)
    Q_WRITABLE_PROPERTY(bool, point_focus_system, false)
};

} // namespace SolTrace::GUI::App

