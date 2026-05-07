#pragma once

#include "components.h"

#include <QObject>
#include <QTimer>

#include <entt/entity/fwd.hpp>

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
        // We HAVE to do this, because entt's callback is "about to be removed"
        // not "removed"
        QTimer::singleShot(
            0, this, [this, entity]() { emit this->removed(entity); });
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
    Component const* get(entt::entity entity) const {
        if (m_host.valid(entity)) {
            return m_host.template try_get<Component>(entity);
        }
        return nullptr;
    }

    template <class F>
    bool try_patch(entt::entity entity, F&& f) {
        if (!m_host.valid(entity)) return false;

        if (m_host.all_of<Component>(entity)) {
            m_host.patch<Component>(entity, f);
            return true;
        }

        return false;
    }

    template <class F>
    void emplace_patch(entt::entity entity, F&& f) {
        if (!m_host.all_of<Component>(entity)) {
            if constexpr (std::is_empty_v<Component>) {
                m_host.emplace<Component>(entity);
            } else {
                m_host.emplace<Component>(entity, Component {});
            }
        }

        m_host.patch<Component>(entity, f);
    }

    auto view() const { return m_host.view<Component const>(); }
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
        db::emplace_patch<Component>(this->m_host, entity, f);
    }

    void remove(entt::entity entity) {
        this->m_host.template remove<Component>(entity);
    }
};

} // namespace db
