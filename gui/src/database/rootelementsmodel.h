#pragma once

#include "database/database_models.h"

#include <QSortFilterProxyModel>

namespace db {

/// A model providing all element entities that do not currently have a parent.
class RootElementsModel : public StructModelAdapter<EntityNamePair> {
    Q_OBJECT

    QPointer<Database> m_host;

    std::unordered_map<entt::entity, int> m_reverse;

    QVector<EntityNamePair> rebuild_lists();

private slots:
    void recompute();
    void record_changed(entt::entity);

public:
    explicit RootElementsModel(QObject* parent = nullptr);
    ~RootElementsModel() override = default;

    void reset(Database* database);

public slots:
    QVariant get(int index);
};

// =============================================================================

/// A model providing all element entities in a database.
class AllElementsModel : public StructModelAdapter<EntityNamePair> {
    Q_OBJECT

    QPointer<Database> m_host;

    std::unordered_map<entt::entity, int> m_reverse;

    QVector<EntityNamePair> rebuild_lists();

private slots:
    void recompute();
    void record_changed(entt::entity);

public:
    explicit AllElementsModel(QObject* parent = nullptr);
    ~AllElementsModel() override = default;

    /// Observe a database and rebuild the element list.
    void reset(Database* database);
};


// =============================================================================

/// Filter proxy for element models by material, geometry, and name.
///
/// Only works with source models that expose EntityNamePair rows backed by
/// Element components.
class InstanceSortFilter : public QSortFilterProxyModel {
    Q_OBJECT

    QPointer<Database> m_host;

    Q_WRITABLE_PROPERTY(Entity, material_filter, { });
    Q_WRITABLE_PROPERTY(Entity, geometry_filter, { });
    Q_WRITABLE_PROPERTY(QString, name_filter, { });

    Q_READONLY_PROPERTY(bool, has_filter);

    Q_READONLY_PROPERTY(QString, material_filter_name);
    Q_READONLY_PROPERTY(QString, geometry_filter_name);

    static constexpr int entity_role = ROLE_FOR_MEMBER(EntityNamePair, entity);

private slots:
    void recompute_has_filter();
    void update_material_name(db::Entity);
    void update_geometry_name(db::Entity);

public:
    explicit InstanceSortFilter(QObject* parent = nullptr);

    /// Observe a database so filters can resolve entity metadata.
    void reset(Database* database);

public slots:
    /// Clear the material-group filter.
    void clear_material();

    /// Clear the geometry-group filter.
    void clear_geometry();

    /// Clear all active filters.
    void clear_all_filters();

    // QSortFilterProxyModel interface
protected:
    bool filterAcceptsRow(int                source_row,
                          QModelIndex const& source_parent) const override;
};

} // namespace db
