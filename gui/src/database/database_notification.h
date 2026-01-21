#pragma once

#include "components.h"

#include <QObject>

#include <entt/entity/fwd.hpp>

namespace db {


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

class ComponentAPIBase : public QObject {
    Q_OBJECT
protected:
    std::weak_ptr<entt::registry> m_host;

public:
    ComponentAPIBase(std::shared_ptr<entt::registry> const& p) : m_host(p) { }
    virtual ~ComponentAPIBase() = default;

signals:
    void changed(entt::entity);
    void removed(entt::entity);
};

template <class Component>
class ComponentAPI : public ComponentAPIBase {
    void change_callback(entt::registry& reg, entt::entity entity) {
        emit changed(entity);
    }

    void remove_callback(entt::registry& reg, entt::entity entity) {
        emit removed(entity);
    }

public:
    ComponentAPI(std::shared_ptr<entt::registry> const& p)
        : ComponentAPIBase(p) {
        p->on_construct<Component>()
            .template connect<&ComponentAPI::change_callback>(this);
        p->on_update<Component>()
            .template connect<&ComponentAPI::change_callback>(this);
        p->on_destroy<Component>()
            .template connect<&ComponentAPI::remove_callback>(this);
    }
    ~ComponentAPI() {
        if (auto l = m_host.lock(); l) {
            l->template on_construct<Component>()
                .template disconnect<&ComponentAPI::change_callback>(this);
            l->template on_update<Component>()
                .template disconnect<&ComponentAPI::change_callback>(this);
            l->template on_destroy<Component>()
                .template disconnect<&ComponentAPI::remove_callback>(this);
        }
    }

    auto* self() { return this; }

    /// Get the content of this component on an entity. Returns null if the
    /// content is not available
    Component* get(entt::entity entity) const {
        if (auto l = m_host.lock(); l) {
            if (l->valid(entity)) {
                return l->template try_get<Component>(entity);
            }
        }
        return nullptr;
    }
};

template <class Component>
class ComponentAPIUpdate : public ComponentAPI<Component> {
public:
    ComponentAPIUpdate(std::shared_ptr<entt::registry> const& p)
        : ComponentAPI<Component>(p) { }
    ~ComponentAPIUpdate() override = default;

public:
    void set(entt::entity entity, Component const& c) {
        if (auto l = this->m_host.lock(); l) {
            if constexpr (std::is_empty_v<Component>) {
                l->template emplace_or_replace<Component>(entity);
            } else {
                l->template emplace_or_replace<Component>(entity, c);
            }
        }
    }

    template <class Function>
    void patch(entt::entity entity, Function&& f) {
        if (auto l = this->m_host.lock(); l) {
            emplace_patch<Component>(*l, entity, f);
        }
    }

    void remove(entt::entity entity) {
        if (auto l = this->m_host.lock(); l) {
            l->template remove<Component>(entity);
        }
    }
};

/// Interface between a database and Qt
class UIApi : public QObject {
    Q_OBJECT

    std::weak_ptr<entt::registry> m_host;

public:
    UIApi(std::shared_ptr<entt::registry> const& p)
        : m_host(p),
          identity(p),
          transform(p),
          invisible(p),
          parent(p),
          tag_root(p),
          group_root(p),
          children(p),
          group(p),
          tags(p) { }
    virtual ~UIApi() = default;

    ComponentAPIUpdate<IdentityComponent>  identity;
    ComponentAPIUpdate<TransformComponent> transform;
    ComponentAPIUpdate<InvisibleComponent> invisible;
    ComponentAPI<ChildOfComponent>         parent;
    ComponentAPI<TagComponent>             tag_root;
    ComponentAPI<GroupComponent>           group_root;
    ComponentAPI<ChildrenComponent>        children;
    ComponentAPI<GroupMemberComponent>     group;
    ComponentAPI<TagMembershipComponent>   tags;

public slots:
    entt::entity add_group(QString               new_name,
                           QVector<entt::entity> members,
                           entt::entity          clone_from = entt::null);
    void         delete_group(entt::entity to_delete,
                              entt::entity move_to = entt::null);
};

struct NotificationResource {
    QSharedPointer<UIApi> notifier;
};

/// Get the notifier for a database. the Notifier is owned by the registry.
UIApi* get_notifier(entt::registry&);

} // namespace db
