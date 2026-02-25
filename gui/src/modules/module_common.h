#pragma once

#include <QObject>

namespace SolTrace::GUI::App {
/**
 * @class StatusComponent
 * @brief Tracks the lifecycle state of an application module.
 *
 * Every app module owns a StatusComponent to communicate its readiness
 * to the QML layer. Status transitions follow a defined lifecycle:
 *
 * @code
 *   Unset → Loading → Incomplete → Ready → Complete → Stale → Error
 * @endcode
 *
 * - Unset:      Initial state, no data loaded
 * - Loading:    Actively reading from disk or backend
 * - Incomplete: Data loaded but required fields are missing
 * - Ready:      Data loaded and validated, ready for use
 * - Complete:   Fully configured and simulation-ready
 * - Stale:      Valid but potentially outdated (backend state changed)
 * - Error:      Validation failed or I/O error occurred
 *
 * QML binds to status to drive loading indicators, error banners,
 * and workflow gating (e.g. disabling "Run" if any module is Incomplete).
 */
class StatusComponent : public QObject {
    Q_OBJECT
public:
    StatusComponent(QObject* parent = nullptr);

    enum class Status {
        Incomplete,
        Loading,
        Ready,
        Error,
        Stale,
        Complete,
        Unset
    };

    Q_ENUM(Status)

    Q_INVOKABLE void mark_incomplete(const QString& message = "");
    Q_INVOKABLE void mark_loading(const QString& message = "");
    Q_INVOKABLE void mark_ready(const QString& message = "Ready");
    Q_INVOKABLE void mark_error(const QString& message);
    Q_INVOKABLE void mark_stale(const QString& message = "");
    Q_INVOKABLE void mark_complete(const QString& message = "");
    Q_INVOKABLE void mark_unset(const QString& message = "");

private:
    Status m_status;
};

} // namespace SolTrace::GUI::App
