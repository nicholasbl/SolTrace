#pragma once

#include <entt/entt.hpp>

#include "database/components.h"
#include "database/database_notification.h"

#include <QPointer>
#include <QtTypes>
#include <qqmlintegration.h>

// TODO BETTER COMPOSITE SUPPORT

namespace db {

/// Helper function: patch a component, skipping it if it does not exist.
/// Returns true if the patch occurred.
template <class Component, class Function>
bool try_patch(entt::registry& reg, entt::entity entity, Function&& f) {
    if (!reg.valid(entity)) return false;

    if (reg.all_of<Component>(entity)) {
        reg.patch<Component>(entity, f);
        return true;
    }

    return false;
}

/// Helper function: patch a component, creating it if it does not exist.
template <class Component, class Function>
void emplace_patch(entt::registry& reg, entt::entity entity, Function&& f) {
    if (!reg.all_of<Component>(entity)) {
        if constexpr (std::is_empty_v<Component>) {
            reg.emplace<Component>(entity);
        } else {
            reg.emplace<Component>(entity, Component {});
        }
    }

    reg.patch<Component>(entity, f);
}

/// Helper function, find a corresponding key to a value in a map
/// Slow, but OK for UI work
template <class K, class V>
std::optional<K> reverse_lookup(std::map<K, V> const& map, V const& value) {
    for (auto const& [k, v] : map) {
        if (v == value) return k;
    }
    return std::nullopt;
}

/// Wrapper type for Qt/QML interface
struct Entity {
    Q_GADGET
    QML_VALUE_TYPE(db_entity);
    // Q_PROPERTY(qint64 value MEMBER value);

public:
    Entity() = default;
    Entity(entt::entity e) : value(e) { }

    entt::entity value = entt::null;

    bool operator<=>(Entity const& other) const = default;

    operator entt::entity() const { return value; }
};

struct DatabaseExport {
    std::shared_ptr<SolTrace::Data::SimulationData>              data;
    std::unordered_map<SolTrace::Data::element_id, entt::entity> element_map;

    DatabaseExport() = default;

    DatabaseExport(DatabaseExport const&)            = delete;
    DatabaseExport& operator=(DatabaseExport const&) = delete;
    DatabaseExport(DatabaseExport&&)                 = default;
    DatabaseExport& operator=(DatabaseExport&&)      = default;
};

class Database : public QObject {
    Q_OBJECT
    entt::registry m_registry;

public:
    /// Create a new simulation database
    explicit Database(QObject* p = nullptr);

    virtual ~Database() = default;

    Database* clone(QObject* p = nullptr) const;

    /// Merge in simulation data. Note, this should be called closely after
    /// the database constructor. We have this split here so that we can
    /// allocate a database on one thread and fill it in another.
    void import(SD::SimulationData&);

    /// Convert a database back into a Soltrace dataset
    std::shared_ptr<DatabaseExport> export_to_simdata();

public:
    operator entt::registry&();
    operator entt::registry const&() const;

    entt::registry&       as_registry();
    entt::registry const& as_registry() const;

public:
    ComponentAPIUpdate<IdentityComponent>  identity;
    ComponentAPIUpdate<TransformComponent> transform;
    ComponentAPI<GlobalTransformComponent> global_transform;
    ComponentAPIUpdate<InvisibleComponent> invisible;
    ComponentAPI<ChildOfComponent>         parent;
    ComponentAPI<TagComponent>             tag_root;

    ComponentAPI<MaterialGroupComponent>       material_root;
    ComponentAPIUpdate<MaterialComponent>      material_parameters;
    ComponentAPI<MaterialGroupMemberComponent> material_group_membership;

    ComponentAPI<GeometryGroupComponent>       geometry_root;
    ComponentAPIUpdate<GeometryComponent>      geometry_parameters;
    ComponentAPI<GeometryGroupMemberComponent> geometry_group_membership;

    ComponentAPI<ChildrenComponent> children;

    ComponentAPI<TagMembershipComponent> tag_membership;

    ComponentAPIUpdate<SelectedComponent> selected;

    ComponentAPIUpdate<ColorComponent> color;

public:
    /// Helper function: patch a component, creating it if it does not exist.
    template <class Component, class Function>
    void emplace_patch(entt::entity entity, Function&& f) {
        emplace_patch(m_registry, entity, f);
    }

public:
    entt::entity create();

    bool valid(entt::entity) const;

    /// Clear the active parent of an entity
    void unset_parent(entt::entity child);

    /// Set the parent of an entity
    void set_parent(entt::entity child, entt::entity parent);

    /// Get the list of children of this entity. Returns an empty list if there
    /// are none.
    std::span<entt::entity const> children_of(entt::entity parent) const;

    /// Get the parent of this entity. Returns entt::null if there is none.
    entt::entity parent_of(entt::entity child) const;

    /// Assign an entity to a material group
    void assign_material(entt::entity child, entt::entity group);

    /// Remove an entity from a material group
    void remove_material(entt::entity child);

    /// Assign an entity to a geometry group
    void assign_geometry(entt::entity child, entt::entity group);

    /// Remove an entity from a geometry group
    void remove_geometry(entt::entity child);

    /// Create a new tag. Note that tag names should be unique.
    entt::entity create_tag(QString name);

    /// Ask if an entity has been given a tag
    bool is_tagged(entt::entity item, entt::entity tag) const;

    /// Assign an entity to a specific tag
    void assign_tag(entt::entity item, entt::entity tag);

    /// Remove a tag from an entity
    void unassign_tag(entt::entity item, entt::entity tag);

    /// Clear and destroy a specific tag
    void delete_tag(entt::entity tag);

    /// Get all the tags for an entity
    std::span<entt::entity const> tags_for(entt::entity item) const;

    /// Get the global ray source of the database
    SD::ray_source_ptr get_ray_source() const;

    /// Get the global simulation parameters
    SD::SimulationParameters const& get_sim_params() const;

public slots:
    /// Get the name of an entity, either using the Identity component, or by
    /// using the entity ID.
    QString name_of(Entity item) const;

    /// Materials
    entt::entity add_material_group(QString               new_name,
                                    QVector<entt::entity> members,
                                    entt::entity clone_from = entt::null);

    void delete_material_group(entt::entity to_delete,
                               entt::entity move_to = entt::null);

    entt::entity material_of(entt::entity element) const;

    /// Geometry
    entt::entity add_geometry_group(QString               new_name,
                                    QVector<entt::entity> members,
                                    entt::entity clone_from = entt::null);

    void delete_geometry_group(entt::entity to_delete,
                               entt::entity move_to = entt::null);

    entt::entity geometry_of(entt::entity element) const;

    /// Selection methods
    void select(entt::entity to_select);

    void deselect(entt::entity to_deselect);

    void toggle_selection(entt::entity to_toggle_selection);

    void clear_selection();

    bool is_selected(entt::entity e) const;

    /// Color
    void set_color(entt::entity to_color, QColor new_color);
};

class DatabaseObserver {

    QPointer<Database>               m_database;
    QVector<QMetaObject::Connection> m_database_conns;

protected:
    void observe(Database* ptr) {
        if (ptr == m_database) return;
        if (m_database) {
            for (auto const& c : std::as_const(m_database_conns)) {
                QObject::disconnect(c);
            }
            m_database_conns.clear();
        }
        m_database = ptr;
        if (ptr) set_new_database_connections(ptr);
    }

    void add_connection(QMetaObject::Connection c) {
        m_database_conns.push_back(c);
    }

    Database*       database() { return m_database; }
    Database const* database() const { return m_database; }

    virtual void set_new_database_connections(Database* ptr) = 0;

    template <class F>
    void with_db(F&& f) {
        if (m_database) { f(m_database); }
    }

public:
    DatabaseObserver()          = default;
    virtual ~DatabaseObserver() = default;

    DatabaseObserver(DatabaseObserver const&)            = delete;
    DatabaseObserver& operator=(DatabaseObserver const&) = delete;
    DatabaseObserver(DatabaseObserver&&)                 = delete;
    DatabaseObserver& operator=(DatabaseObserver&&)      = delete;
};

} // namespace db

Q_DECLARE_METATYPE(db::Entity);
