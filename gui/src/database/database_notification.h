#pragma once

#include "components.h"

#include <QObject>

#include <entt/entity/fwd.hpp>

namespace db {

class ComponentAPIBase : public QObject {
    Q_OBJECT
protected:
    entt::registry& m_host;

public:
    ComponentAPIBase(entt::registry& p) : m_host(p) { }
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
    ComponentAPI(entt::registry& p) : ComponentAPIBase(p) {
        p.on_construct<Component>()
            .template connect<&ComponentAPI::change_callback>(this);
        p.on_update<Component>()
            .template connect<&ComponentAPI::change_callback>(this);
        p.on_destroy<Component>()
            .template connect<&ComponentAPI::remove_callback>(this);
    }
    ~ComponentAPI() {
        m_host.template on_construct<Component>()
            .template disconnect<&ComponentAPI::change_callback>(this);
        m_host.template on_update<Component>()
            .template disconnect<&ComponentAPI::change_callback>(this);
        m_host.template on_destroy<Component>()
            .template disconnect<&ComponentAPI::remove_callback>(this);
    }

    auto* self() { return this; }

    /// Get the content of this component on an entity. Returns null if the
    /// content is not available
    Component* get(entt::entity entity) const {
        if (m_host.valid(entity)) {
            return m_host.template try_get<Component>(entity);
        }
        return nullptr;
    }
};

template <class Component>
class ComponentAPIUpdate : public ComponentAPI<Component> {
public:
    ComponentAPIUpdate(entt::registry& p) : ComponentAPI<Component>(p) { }
    ~ComponentAPIUpdate() override = default;

public:
    void set(entt::entity entity, Component const& c) {
        if constexpr (std::is_empty_v<Component>) {
            this->m_host.template emplace_or_replace<Component>(entity);
        } else {
            this->m_host.template emplace_or_replace<Component>(entity, c);
        }
    }

    template <class Function>
    void patch(entt::entity entity, Function&& f) {
        emplace_patch<Component>(this->m_host, entity, f);
    }

    void remove(entt::entity entity) {
        this->m_host.template remove<Component>(entity);
    }
};

} // namespace db
