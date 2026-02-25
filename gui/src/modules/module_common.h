#pragma once

#include <QObject>
#include <QAbstractListModel>

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

/**
 * @class PresetComponentBase
 * @brief QML-facing base class for preset management.
 *
 * Exposes preset operations to QML via Q_INVOKABLE methods.
 * Inherits QAbstractListModel so QML can display the list of available
 * presets directly (name + description per row).
 *
 * Actual file I/O is delegated to the backend's file utilities service.
 * This class is the thin QML adapter — it does not own persistence logic.
 *
 * Roles:
 * - NameRole:        Display name of the preset
 * - DescriptionRole: Optional description string
 */
class PresetComponentBase : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles { NameRole = Qt::UserRole + 1, DescriptionRole };

    Q_INVOKABLE bool load(const QString& name);
    Q_INVOKABLE bool save(const QString& name, const QString& description = "");
    Q_INVOKABLE bool remove(const QString& name);
    Q_INVOKABLE bool import_preset(const QString& filepath);
    Q_INVOKABLE bool export_preset(const QString& name,
                                   const QString& filepath);

    int      rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index,
                                int                role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
};

/**
 * @class PresetComponent
 * @brief Typed preset storage and active selection management.
 *
 * Template subclass of PresetComponentBase that adds typed storage
 * and tracks the currently active preset.
 *
 * @tparam T The domain type being managed (e.g. SunDefinition,
 * DirectionalSunPosition)
 *
 * active_preset reflects the currently loaded configuration. QML binds
 * to this to display and edit the active parameters. Changing active_preset
 * does not automatically save — the user must explicitly call save().
 *
 * m_keys preserves insertion order for stable row indexing in the list model.
 */

template <typename T>
class PresetComponent : public PresetComponentBase {
public:
    QPointer<T> active() const;
    void        set_active(T* preset);

private:
    QHash<QString, T*> m_presets;
    QList<QString>     m_keys;
    QPointer<T>        m_active_preset;
};

} // namespace SolTrace::GUI::App
