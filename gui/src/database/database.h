#pragma once

#include <entt/entt.hpp>

#include "utilities/indirect_model.h"

#include <QObject>
#include <QQuaternion>
#include <QVector3D>

#include "simulation_data_api.hpp"

// TODO BETTER COMPOSITE SUPPORT

namespace SD = SolTrace::Data;

bool operator==(SD::OpticalProperties const& a, SD::OpticalProperties const& b);

namespace db {

struct DisabledComponent { };

struct ChildOfComponent {
    entt::entity parent;
};

struct ChildrenComponent {
    std::unordered_set<entt::entity> children;
};

struct PositionComponent {
    QVector3D position;
};

struct OrientationComponent {
    QQuaternion rotation;
};

struct RaySourceResource {
    SD::ray_source_ptr source;
};

struct GroupParameters {
    SD::aperture_ptr aperture;
    SD::surface_ptr  surface;

    SD::OpticalProperties optics_front;
    SD::OpticalProperties optics_back;

    bool operator==(GroupParameters const&) const = default;
};

struct GroupComponent {
    QString name;

    GroupParameters parameters;

    std::unordered_set<entt::entity> members;
};

struct GroupMemberComponent {
    entt::entity group;
};


// =============================================================================

struct CachedGroup {
    QString      name;
    entt::entity entity;
};

class UIApi : public QObject {
    Q_OBJECT

    std::weak_ptr<entt::registry> m_host;

public:
    UIApi(std::shared_ptr<entt::registry> const& p) : m_host(p) { }
    virtual ~UIApi() = default;

signals:
    void group_changed(db::CachedGroup);
    void group_removed(entt::entity);

public slots:
    void         change_group_name(entt::entity, QString);
    entt::entity add_group(QString new_name, QVector<entt::entity>);
    void         delete_group(entt::entity to_delete, entt::entity move_to);
};

struct NotificationResource {
    QSharedPointer<UIApi> notifier;
};

// =============================================================================

static std::shared_ptr<entt::registry>     create_new();
static std::shared_ptr<entt::registry>     import(SD::SimulationData&);
static std::shared_ptr<SD::SimulationData> export_to_simdata(entt::registry&);

void unset_parent(entt::registry&, entt::entity child);
void set_parent(entt::registry&, entt::entity child, entt::entity parent);
std::unordered_set<entt::entity>* children_of(entt::registry&,
                                              entt::entity parent);

void assign_group(entt::registry&, entt::entity child, entt::entity group);

SD::ray_source_ptr        get_ray_source(entt::registry const&);
SD::SimulationParameters& get_sim_params(entt::registry&);


} // namespace db
