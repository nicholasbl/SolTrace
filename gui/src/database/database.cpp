#include "database.h"

#include <algorithm>
#include <cmath>
#include <unordered_map>

template <class T>
void hash_combine(size_t& seed, T const& v) {
    seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <class T>
std::optional<T> optional_ptr(T* ptr) {
    if (ptr) return *ptr;
    return std::nullopt;
}

template <>
struct std::hash<SD::OpticalProperties> {
    std::size_t operator()(SD::OpticalProperties const& a) const {
        size_t seed = 0;

        hash_combine(seed, a.my_type);
        hash_combine(seed, a.error_distribution_type);
        hash_combine(seed, a.transmitivity);
        hash_combine(seed, a.reflectivity);
        hash_combine(seed, a.slope_error);
        hash_combine(seed, a.specularity_error);
        hash_combine(seed, a.refraction_index_front);
        hash_combine(seed, a.refraction_index_back);

        return seed;
    }
};

bool operator==(SD::OpticalProperties const& a,
                SD::OpticalProperties const& b) {
    return std::tie(a.my_type,
                    a.error_distribution_type,
                    a.transmitivity,
                    a.reflectivity,
                    a.slope_error,
                    a.specularity_error,
                    a.refraction_index_front,
                    a.refraction_index_back) ==
           std::tie(b.my_type,
                    b.error_distribution_type,
                    b.transmitivity,
                    b.reflectivity,
                    b.slope_error,
                    b.specularity_error,
                    b.refraction_index_front,
                    b.refraction_index_back);
}

template <>
struct std::hash<db::GroupParameters> {
    std::size_t operator()(db::GroupParameters const& a) const {
        size_t seed = 0;

        // this hashes the pointer, which should be ok
        hash_combine(seed, a.aperture);
        hash_combine(seed, a.surface);
        hash_combine(seed, a.optics_front);
        hash_combine(seed, a.optics_back);

        return seed;
    }
};

namespace db {

QQuaternion convert(SD::Matrix3d const& m) {

    auto mat = QMatrix3x3(Qt::Initialization::Uninitialized);

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            mat(i, j) = m.get_value(i, j);
        }
    }

    return QQuaternion::fromRotationMatrix(mat);
}

SD::Matrix3d convert(QQuaternion const& m) {

    SD::Matrix3d mat;

    auto from = m.toRotationMatrix();

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            mat.set_value(i, j, from(i, j));
        }
    }

    return mat;
}

// =============================================================================

void UIApi::change_group_name(entt::entity entity, QString name) {
    auto lock = m_host.lock();
    if (!lock) return;

    if (!lock->all_of<GroupComponent>(entity)) { return; }

    lock->patch<GroupComponent>(entity,
                                [&name](GroupComponent& a) { a.name = name; });
}

entt::entity UIApi::add_group(QString new_name, QVector<entt::entity> members) {
    auto lock = m_host.lock();
    if (!lock) return entt::null;

    auto set = std::unordered_set(members.begin(), members.end());

    auto ent = lock->create();
    lock->emplace<GroupComponent>(ent,
                                  GroupComponent {
                                      .name    = new_name,
                                      .members = set,
                                  });

    for (auto child : set) {
        lock->emplace_or_replace<GroupMemberComponent>(
            child, GroupMemberComponent { .group = ent });
    }

    return ent;
}

void UIApi::delete_group(entt::entity to_delete, entt::entity move_to) {
    auto lock = m_host.lock();
    if (!lock) return;

    if (!lock->all_of<GroupComponent>(to_delete)) { return; }

    auto members = std::move(lock->get<GroupComponent>(to_delete).members);

    lock->destroy(to_delete);

    for (auto child : members) {
        lock->emplace_or_replace<GroupMemberComponent>(
            child, GroupMemberComponent { .group = move_to });
    }

    if (!lock->all_of<GroupComponent>(move_to)) {
        lock->emplace<GroupComponent>(
            move_to,
            GroupComponent {
                .name    = QString("Group {}").arg(entt::to_integral(move_to)),
                .members = std::move(members),
            });
    } else {
        lock->patch<GroupComponent>(move_to, [&](GroupComponent& a) {
            a.members.insert(members.begin(), members.end());
        });
    }
}

// =============================================================================

static void on_group_change(entt::registry& reg, entt::entity entity) {
    auto* nptr = reg.ctx().find<NotificationResource>();
    if (!nptr) return;

    auto* gptr = reg.try_get<GroupComponent>(entity);
    if (!gptr) { return; }

    nptr->notifier->group_changed(CachedGroup {
        .name   = gptr->name,
        .entity = entity,
    });
}

static void on_group_remove(entt::registry& reg, entt::entity entity) {
    auto* nptr = reg.ctx().find<NotificationResource>();
    if (!nptr) return;

    nptr->notifier->group_removed(entity);
}

// =============================================================================

std::shared_ptr<entt::registry> create_new() {
    auto ret = std::make_shared<entt::registry>();

    ret->ctx().emplace<NotificationResource>(NotificationResource {
        .notifier = QSharedPointer<UIApi>(new UIApi(ret)),
    });


    ret->on_construct<GroupComponent>().connect<&on_group_change>();
    ret->on_update<GroupComponent>().connect<&on_group_change>();
    ret->on_destroy<GroupComponent>().connect<&on_group_remove>();

    return ret;
}

std::shared_ptr<entt::registry> import(SD::SimulationData& data) {
    auto ret = create_new();

    // we use pointers as element IDs are scoped to global AND composite
    std::unordered_map<SD::Element*, entt::entity> emap;

    auto get_or_create_entity = [&](SD::Element* ptr) {
        if (auto eiter = emap.find(ptr); eiter != emap.end()) {
            return eiter->second;
        } else {
            auto ent  = ret->create();
            emap[ptr] = ent;

            return ent;
        }
    };

    std::unordered_map<GroupParameters, entt::entity> groups;

    size_t group_counter = 0;

    for (auto iter = data.get_iterator(); !data.is_at_end(iter); ++iter) {
        auto const& element = *(iter->second);

        entt::entity ent = get_or_create_entity(iter->second.get());


        auto position = element.get_origin_ref();

        auto rotation = convert(element.get_local_to_reference());

        ret->emplace<PositionComponent>(
            ent,
            PositionComponent {
                .position = QVector3D(
                    position.data[0], position.data[1], position.data[2]),
            });

        ret->emplace<OrientationComponent>(
            ent, OrientationComponent { .rotation = rotation });


        if (!element.is_enabled()) { ret->emplace<DisabledComponent>(ent); }

        if (element.is_composite()) {
            auto& c = *static_cast<SD::CompositeElement const*>(&element);

            for (auto iter = c.get_const_iterator(); !c.is_at_end(iter);
                 ++iter) {

                auto child_ent = get_or_create_entity(iter->second.get());

                if (ret->any_of<ChildOfComponent>(child_ent)) {
                    // Uh oh. We should not have multiple parents!
                    throw std::runtime_error("Multiple parents for element");
                }

                set_parent(*ret, child_ent, ent);
            }

            // OK now how do we assign a group to this element?

            continue;
        }

        if (!element.get_front_optical_properties() or
            !element.get_back_optical_properties()) {
            continue;
        }

        auto local = GroupParameters {
            .aperture     = element.get_aperture(),
            .surface      = element.get_surface(),
            .optics_front = *element.get_front_optical_properties(),
            .optics_back  = *element.get_back_optical_properties(),
        };

        if (auto group = groups.find(local); group != groups.end()) {
            // we have such a group

            assign_group(*ret, ent, group->second);

        } else {
            // no such group

            auto new_group = GroupComponent {
                .name       = QString("Group %1").arg(group_counter),
                .parameters = local,
            };

            group_counter++;

            auto group_entity = ret->create();

            ret->emplace<GroupComponent>(group_entity, new_group);

            groups.try_emplace(local, group_entity);

            assign_group(*ret, ent, group_entity);
        }
    }

    // fill in missing group information

    // TODO: this will probably fail with nested composites

    {
        // if all the children are in the same group, duplicate
        // if mixed, just pick the first one
        auto view = ret->view<const ChildrenComponent>(
            entt::exclude<GroupMemberComponent, GroupComponent>);

        // all entities with no group
        for (auto [parent, children] : view.each()) {

            // get all groups of the children
            std::unordered_set<entt::entity> children_groups;

            for (auto child : children.children) {
                auto* comp = ret->try_get<GroupMemberComponent>(child);

                if (comp) { children_groups.insert(comp->group); }
            }

            // if there are groups we can assign
            if (children_groups.size()) {
                assign_group(*ret, parent, *children_groups.begin());
            }
        }
    }

    ret->ctx().emplace<RaySourceResource>(RaySourceResource {
        .source = data.get_ray_source(),
    });

    ret->ctx().emplace<SD::SimulationParameters>(
        data.get_simulation_parameters());

    return ret;
}

std::shared_ptr<SD::SimulationData> export_to_simdata(entt::registry& reg) {
    SD::SimulationData ret;

    auto param_ptr = reg.ctx().find<SD::SimulationParameters>();
    if (!param_ptr) return nullptr;

    ret.get_simulation_parameters() = *param_ptr;

    auto ray_source_ptr = reg.ctx().find<RaySourceResource>();
    if (!ray_source_ptr) return nullptr;

    ret.add_ray_source(ray_source_ptr->source);

    auto view = reg.view<const GroupComponent>();

    for (auto const& [e, group] : view.each()) {
        for (auto child : group.members) {
            auto ptr = std::make_shared<SD::SingleElement>();

            ptr->set_aperture(group.parameters.aperture);
            ptr->set_surface(group.parameters.surface);

            ptr->set_front_optical_properties(group.parameters.optics_front);
            ptr->set_back_optical_properties(group.parameters.optics_back);

            QVector3D origin(0.0f, 0.0f, 0.0f);
            if (auto pos_comp = reg.try_get<PositionComponent>(child);
                pos_comp) {
                origin = pos_comp->position;
                ptr->set_origin(origin.x(), origin.y(), origin.z());
            }

            if (auto rot_comp = reg.try_get<OrientationComponent>(child);
                rot_comp) {
                auto rot = rot_comp->rotation.normalized().toRotationMatrix();

                QVector3D dir(rot(0, 2), rot(1, 2), rot(2, 2));
                if (!qFuzzyIsNull(dir.lengthSquared())) { dir.normalize(); }

                auto aim = origin + dir;
                ptr->set_aim_vector(aim.x(), aim.y(), aim.z());

                auto ref_to_local = rot.transposed();
                double beta = std::asin(std::clamp(
                    static_cast<double>(ref_to_local(2, 1)), -1.0, 1.0));
                double cosb = std::cos(beta);
                double gamma = 0.0;
                if (std::abs(cosb) > 1e-6) {
                    gamma = std::atan2(-ref_to_local(0, 1),
                                       ref_to_local(1, 1));
                }
                ptr->set_zrot_radians(gamma);
            }

            if (reg.any_of<DisabledComponent>(child)) { ptr->disable(); }


            ret.add_element(ptr);
        }
    }

    return std::make_shared<SD::SimulationData>(std::move(ret));
}

void unset_parent(entt::registry& reg, entt::entity child) {

    auto child_comp = reg.try_get<ChildOfComponent>(child);

    if (!child_comp) return;

    auto has_parent_comp = reg.all_of<ChildrenComponent>(child_comp->parent);

    if (!has_parent_comp) {
        reg.erase<ChildOfComponent>(child);
        return;
    }

    reg.erase<ChildOfComponent>(child);

    reg.patch<ChildrenComponent>(
        child_comp->parent,
        [child](ChildrenComponent& c) { c.children.erase(child); });
}

void set_parent(entt::registry& reg, entt::entity child, entt::entity parent) {
    // remove existing parent, if any
    unset_parent(reg, child);

    if (!reg.all_of<ChildrenComponent>(parent)) {
        // if the parent doesnt have a child component, insert this as our first
        // child
        reg.emplace<ChildrenComponent>(parent,
                                       ChildrenComponent {
                                           .children = { child },
                                       });
    } else {
        // it has a child component, add to it
        reg.patch<ChildrenComponent>(parent, [child](ChildrenComponent& a) {
            a.children.insert(child);
        });
    }

    // set the child's parent
    reg.emplace<ChildOfComponent>(child, ChildOfComponent { .parent = parent });
}

std::unordered_set<entt::entity>* children_of(entt::registry& reg,
                                              entt::entity    parent) {
    auto parent_comp = reg.try_get<ChildrenComponent>(parent);

    if (!parent_comp) { return nullptr; }

    return &(parent_comp->children);
}

void unset_group(entt::registry& reg, entt::entity child) {
    // is this a member of a group?
    auto child_comp = reg.try_get<GroupMemberComponent>(child);

    if (!child_comp) return;

    // Check if the group parent exists
    auto parent_comp = reg.try_get<GroupComponent>(child_comp->group);

    // remove us from a member of the group
    reg.erase<GroupMemberComponent>(child);

    if (!parent_comp) { return; }

    // remove us from the parent
    parent_comp->members.erase(child);
}

void assign_group(entt::registry& reg, entt::entity child, entt::entity group) {
    unset_group(reg, child);

    reg.emplace_or_replace<GroupMemberComponent>(
        child, GroupMemberComponent { .group = group });

    reg.patch<GroupComponent>(
        group, [child](GroupComponent& c) { c.members.insert(child); });
}

SD::ray_source_ptr get_ray_source(entt::registry const& reg) {
    if (auto ptr = reg.ctx().find<RaySourceResource>(); ptr) {
        return ptr->source;
    }

    return {};
}

SD::SimulationParameters& get_sim_params(entt::registry& reg) {
    if (auto ptr = reg.ctx().find<SD::SimulationParameters>(); ptr) {
        return *ptr;
    }

    throw std::runtime_error("missing simulation parameters");
}

} // namespace db
