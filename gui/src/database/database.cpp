#include "database.h"
#include "database/components.h"
#include "database/database_notification.h"

#include "simulation_data_api.hpp"

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

std::shared_ptr<entt::registry> create_new() {
    auto ret = std::make_shared<entt::registry>();

    ret->ctx().emplace<NotificationResource>(NotificationResource {
        .notifier = QSharedPointer<UIApi>(new UIApi(ret)),
    });

    return ret;
}

std::shared_ptr<entt::registry> import(SD::SimulationData& data) {
    auto ret = create_new();

    auto imported_tag = create_tag(*ret, "imported");

    // we use pointers as element IDs are scoped to global AND composite
    std::unordered_map<SD::Element*, entt::entity> emap;

    auto get_or_create_entity = [&](SD::Element* ptr) {
        if (auto eiter = emap.find(ptr); eiter != emap.end()) {
            return eiter->second;
        } else {
            auto ent  = ret->create();
            emap[ptr] = ent;

            assign_tag(*ret, ent, imported_tag);

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

        ret->emplace<TransformComponent>(
            ent,
            TransformComponent {
                .position = QVector3D(
                    position.data[0], position.data[1], position.data[2]),
                .rotation = rotation,
            });


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
                .parameters = std::make_shared<GroupParameters>(local),
            };

            auto group_entity = ret->create();

            ret->emplace<GroupComponent>(group_entity, new_group);
            ret->emplace<IdentityComponent>(
                group_entity,
                IdentityComponent {
                    .name = QString("Group %1").arg(group_counter),
                });

            group_counter++;

            groups.try_emplace(local, group_entity);

            assign_group(*ret, ent, group_entity);
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

            ptr->set_aperture(group.parameters->aperture);
            ptr->set_surface(group.parameters->surface);

            ptr->set_front_optical_properties(group.parameters->optics_front);
            ptr->set_back_optical_properties(group.parameters->optics_back);

            QVector3D origin(0.0f, 0.0f, 0.0f);
            if (auto tf_comp = reg.try_get<TransformComponent>(child);
                tf_comp) {
                origin = tf_comp->position;
                ptr->set_origin(origin.x(), origin.y(), origin.z());

                auto rot = tf_comp->rotation.normalized().toRotationMatrix();

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
        [child](ChildrenComponent& c) { erase(c.children, child); });
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
            a.children.push_back(child);
        });
    }

    // set the child's parent
    reg.emplace<ChildOfComponent>(child, ChildOfComponent { .parent = parent });
}

std::span<entt::entity const> children_of(entt::registry& reg,
                                          entt::entity    parent) {
    auto parent_comp = reg.try_get<ChildrenComponent>(parent);

    if (!parent_comp) { return {}; }

    return parent_comp->children;
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
    erase(parent_comp->members, child);
}

void assign_group(entt::registry& reg, entt::entity child, entt::entity group) {
    unset_group(reg, child);

    reg.emplace_or_replace<GroupMemberComponent>(
        child, GroupMemberComponent { .group = group });

    reg.patch<GroupComponent>(
        group, [child](GroupComponent& c) { c.members.push_back(child); });
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

entt::entity create_tag(entt::registry& reg, QString name) {
    auto ret = reg.create();

    reg.emplace<TagComponent>(ret);
    reg.emplace<IdentityComponent>(ret, IdentityComponent { .name = name });

    return ret;
}

bool is_tagged(entt::registry& reg, entt::entity item, entt::entity tag) {
    if (auto* ptr = reg.try_get<TagMembershipComponent>(item); ptr) {
        auto& t = ptr->tags;

        return std::find(t.begin(), t.end(), tag) == t.end();
    }
    return false;
}

void assign_tag(entt::registry& reg, entt::entity item, entt::entity tag) {
    if (!reg.all_of<TagComponent>(tag)) { return; }

    if (is_tagged(reg, item, tag)) return;

    auto& storage = reg.storage<ATagMemberComponent>(entt::to_integral(tag));

    storage.emplace(item);

    emplace_patch<TagMembershipComponent>(
        reg, item, [tag](TagMembershipComponent& tc) {
            tc.tags.push_back(tag);
        });
}

void unassign_tag(entt::registry& reg, entt::entity item, entt::entity tag) {
    if (!reg.all_of<TagComponent>(tag)) { return; }

    if (!reg.all_of<TagMembershipComponent>(item)) { return; }

    reg.patch<TagMembershipComponent>(
        item, [tag](TagMembershipComponent& tc) { erase(tc.tags, tag); });

    auto& storage = reg.storage<ATagMemberComponent>(entt::to_integral(tag));

    if (storage.contains(item)) { storage.erase(item); }

    if (storage.empty()) { reg.reset(entt::to_integral(tag)); }
}

void delete_tag(entt::registry& reg, entt::entity tag) {
    auto& storage = reg.storage<ATagMemberComponent>(entt::to_integral(tag));

    for (auto x : storage) {
        erase(reg.get<TagMembershipComponent>(x).tags, tag);
    }

    reg.reset(entt::to_integral(tag));

    reg.destroy(tag);
}

std::span<entt::entity const> tags_for(entt::registry& reg, entt::entity item) {
    if (auto ptr = reg.try_get<TagMembershipComponent>(item); ptr) {
        return ptr->tags;
    }

    return {};
}

QString name_of(entt::registry& reg, entt::entity item) {
    if (!reg.valid(item)) return {};

    if (auto ptr = reg.try_get<IdentityComponent>(item); ptr) {
        return ptr->name;
    }

    return QString("Entity %1").arg(entt::to_integral(item));
}

UIApi* get_notifier(entt::registry& reg) {
    auto ptr = reg.ctx().find<NotificationResource>();

    if (!ptr) return nullptr;

    return ptr->notifier.get();
}

} // namespace db
