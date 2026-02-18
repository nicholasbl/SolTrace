#pragma once

#include <entt/entt.hpp>

#include "database/components.h"
#include "database/database_notification.h"

#include <QPointer>


// TODO BETTER COMPOSITE SUPPORT

namespace db {


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


class Database : public QObject {
    entt::registry m_registry;

public:
    /// Create a new simulation database
    explicit Database(QObject* p = nullptr);

    virtual ~Database() = default;

    /// Merge in simulation data. Note, this should be called closely after
    /// the database constructor. We have this split here so that we can
    /// allocate a database on one thread and fill it in another.
    void import(SD::SimulationData&);

    /// Convert a database back into a Soltrace dataset
    std::shared_ptr<SolTrace::Data::SimulationData> export_to_simdata();

public:
    operator entt::registry&() { return m_registry; }
    operator entt::registry const&() const { return m_registry; }

    entt::registry&       as_registry() { return m_registry; }
    entt::registry const& as_registry() const { return m_registry; }

public:
    ComponentAPIUpdate<IdentityComponent>             identity;
    ComponentAPIUpdate<TransformComponent>            transform;
    ComponentAPIUpdate<InvisibleComponent>            invisible;
    ComponentAPI<ChildOfComponent>                    parent;
    ComponentAPI<TagComponent>                        tag_root;
    ComponentAPI<RenderGroupComponent>                group_root;
    ComponentAPIUpdate<RenderGroupParameterComponent> group_parameters;
    ComponentAPI<ChildrenComponent>                   children;
    ComponentAPI<RenderGroupMemberComponent>          group_membership;
    ComponentAPI<TagMembershipComponent>              tag_membership;

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

    /// Assign an entity to a geometry group
    void assign_group(entt::entity child, entt::entity group);

    /// Remove an entity from a geometry group
    void unset_group(entt::entity child);

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

    /// Get the name of an entity, either using the Identity component, or by
    /// using the entity ID.
    QString name_of(entt::entity item) const;

    /// Get the global ray source of the database
    SD::ray_source_ptr get_ray_source() const;

    /// Get the global simulation parameters
    SD::SimulationParameters const& get_sim_params() const;

    TransformComponent global_transform(entt::entity item) const;


public slots:
    entt::entity add_render_group(QString               new_name,
                                  QVector<entt::entity> members,
                                  entt::entity clone_from = entt::null);

    void delete_render_group(entt::entity to_delete,
                             entt::entity move_to = entt::null);
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
