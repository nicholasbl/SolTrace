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


template <>
struct std::hash<db::GroupParameters> {
    std::size_t operator()(db::GroupParameters const& a) const {
        size_t seed = 0;

        hash_combine(seed, *a.aperture);
        hash_combine(seed, *a.surface);
        hash_combine(seed, a.optics_front);
        hash_combine(seed, a.optics_back);

        return seed;
    }
};

static bool is_equal(SD::aperture_ptr const& a, SD::aperture_ptr const& b);
static bool is_equal(SD::surface_ptr const& a, SD::surface_ptr const& b);

static size_t hash_aperture(SD::Aperture const& a);
static size_t hash_surface(SD::Surface const& a);

template <>
struct std::hash<SD::Aperture> {
    std::size_t operator()(SD::Aperture const& a) const {
        return hash_aperture(a);
    }
};

template <>
struct std::hash<SD::Surface> {
    std::size_t operator()(SD::Surface const& a) const {
        return hash_surface(a);
    }
};


// --- Hashing ----------------------------------------------------------------

static size_t hash_aperture(SD::Aperture const& a) {
    size_t seed = 0;
    hash_combine(seed, a.my_type);

    switch (a.my_type) {
    case SolTrace::Data::ANNULUS: {
        auto* aa = dynamic_cast<SD::Annulus const*>(&a);
        if (!aa) break;
        hash_combine(seed, aa->inner_radius);
        hash_combine(seed, aa->outer_radius);
        hash_combine(seed, aa->arc_angle);
        break;
    }
    case SolTrace::Data::CIRCLE: {
        auto* aa = dynamic_cast<SD::Circle const*>(&a);
        if (!aa) break;
        hash_combine(seed, aa->diameter);
        break;
    }
    case SolTrace::Data::HEXAGON: {
        auto* aa = dynamic_cast<SD::Hexagon const*>(&a);
        if (!aa) break;
        hash_combine(seed, aa->circumscribe_diameter);
        break;
    }
    case SolTrace::Data::RECTANGLE: {
        auto* aa = dynamic_cast<SD::Rectangle const*>(&a);
        if (!aa) break;
        hash_combine(seed, aa->x_length);
        hash_combine(seed, aa->y_length);
        hash_combine(seed, aa->x_coord);
        hash_combine(seed, aa->y_coord);
        break;
    }
    case SolTrace::Data::EQUILATERAL_TRIANGLE: {
        auto* aa = dynamic_cast<SD::EqualateralTriangle const*>(&a);
        if (!aa) break;
        hash_combine(seed, aa->circumscribe_diameter);
        break;
    }
    case SolTrace::Data::SINGLE_AXIS_CURVATURE_SECTION:
        break;
    case SolTrace::Data::IRREGULAR_TRIANGLE: {
        auto* aa = dynamic_cast<SD::IrregularTriangle const*>(&a);
        if (!aa) break;
        hash_combine(seed, aa->x1);
        hash_combine(seed, aa->y1);
        hash_combine(seed, aa->x2);
        hash_combine(seed, aa->y2);
        hash_combine(seed, aa->x3);
        hash_combine(seed, aa->y3);
        break;
    }
    case SolTrace::Data::IRREGULAR_QUADRILATERAL: {
        auto* aa = dynamic_cast<SD::IrregularQuadrilateral const*>(&a);
        if (!aa) break;
        hash_combine(seed, aa->x1);
        hash_combine(seed, aa->y1);
        hash_combine(seed, aa->x2);
        hash_combine(seed, aa->y2);
        hash_combine(seed, aa->x3);
        hash_combine(seed, aa->y3);
        hash_combine(seed, aa->x4);
        hash_combine(seed, aa->y4);
        break;
    }
    case SolTrace::Data::APERTURE_UNKNOWN:
        break;
    }

    return seed;
}

static size_t hash_surface(SD::Surface const& a) {
    size_t seed = 0;
    hash_combine(seed, a.my_type);

    switch (a.my_type) {
    case SolTrace::Data::CONE: {
        auto* aa = dynamic_cast<SD::Cone const*>(&a);
        if (!aa) break;
        hash_combine(seed, aa->half_angle);
        break;
    }
    case SolTrace::Data::CYLINDER: {
        auto* aa = dynamic_cast<SD::Cylinder const*>(&a);
        if (!aa) break;
        hash_combine(seed, aa->radius);
        break;
    }
    case SolTrace::Data::FLAT:
        break;
    case SolTrace::Data::PARABOLA: {
        auto* aa = dynamic_cast<SD::Parabola const*>(&a);
        if (!aa) break;
        hash_combine(seed, aa->focal_length_x);
        hash_combine(seed, aa->focal_length_y);
        break;
    }
    case SolTrace::Data::SPHERE: {
        auto* aa = dynamic_cast<SD::Sphere const*>(&a);
        if (!aa) break;
        hash_combine(seed, aa->vertex_curv);
        break;
    }
    case SolTrace::Data::HYPER:
    case SolTrace::Data::GENERAL_SPENCER_MURTY:
    case SolTrace::Data::TORUS:
    case SolTrace::Data::SURFACE_UNKNOWN:
        break;
    }

    return seed;
}


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

void import_optics(entt::registry&                                    reg,
                   entt::entity                                       entity,
                   SD::Element const&                                 item,
                   std::unordered_map<GroupParameters, entt::entity>& groups,
                   size_t& group_counter) {

    if (!item.get_front_optical_properties() and
        !item.get_back_optical_properties() and !item.get_aperture() and
        !item.get_surface()) {
        // does not have geometry.
        return;
    }

    if (!item.get_front_optical_properties() or
        !item.get_back_optical_properties() or !item.get_aperture() or
        !item.get_surface()) {

        emplace_patch<ImportErrorComponent>(reg, entity, [](auto& c) {
            c.reason += "Missing aperture and surface";
        });
        return;
    }

    auto local = GroupParameters {
        .aperture     = item.get_aperture(),
        .surface      = item.get_surface(),
        .optics_front = *item.get_front_optical_properties(),
        .optics_back  = *item.get_back_optical_properties(),
    };

    if (auto group = groups.find(local); group != groups.end()) {
        // we have such a group

        assign_group(reg, entity, group->second);

    } else {
        // no such group

        auto new_group = GroupComponent {
            .parameters = std::make_shared<GroupParameters>(local),
        };

        auto group_entity = reg.create();

        reg.emplace<GroupComponent>(group_entity, new_group);
        reg.emplace<IdentityComponent>(
            group_entity,
            IdentityComponent {
                .name = QString("Group %1").arg(group_counter),
            });

        group_counter++;

        groups.try_emplace(local, group_entity);

        assign_group(reg, entity, group_entity);
    }
}

std::shared_ptr<entt::registry> import(SD::SimulationData& data) {
    auto ret = create_new();

    // Assuming we are not re-using registries, which we are not for the moment
    auto imported_tag = create_tag(*ret, "imported");

    // we use pointers as element IDs are scoped to global AND composite
    std::unordered_map<SD::Element*, entt::entity> emap;

    auto get_or_create_entity = [&](SD::Element* ptr) {
        if (auto eiter = emap.find(ptr); eiter != emap.end()) {
            return eiter->second;
        } else {
            auto ent  = ret->create();
            emap[ptr] = ent;

            ret->emplace<ElementComponent>(ent);

            assign_tag(*ret, ent, imported_tag);

            return ent;
        }
    };

    // MAP MAY NOT BE RE-USED!
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

        if (!element.get_name().empty()) {
            ret->emplace<IdentityComponent>(
                ent, QString::fromStdString(element.get_name()));
        }

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
        }

        import_optics(*ret, ent, element, groups, group_counter);
    }

    ret->ctx().emplace<RaySourceResource>(RaySourceResource {
        .source = data.get_ray_source(),
    });

    ret->ctx().emplace<SD::SimulationParameters>(
        data.get_simulation_parameters());


    return ret;
}

// TODO: remove once we have sorted the vector aim thing
static void install_transform(SD::element_ptr           ptr,
                              TransformComponent const& tf_comp) {
    auto origin = tf_comp.position;
    ptr->set_origin(origin.x(), origin.y(), origin.z());

    auto rot = tf_comp.rotation.normalized().toRotationMatrix();

    QVector3D dir(rot(0, 2), rot(1, 2), rot(2, 2));
    if (!qFuzzyIsNull(dir.lengthSquared())) { dir.normalize(); }

    auto aim = origin + dir;
    ptr->set_aim_vector(aim.x(), aim.y(), aim.z());

    auto   ref_to_local = rot.transposed();
    double beta         = std::asin(
        std::clamp(static_cast<double>(ref_to_local(2, 1)), -1.0, 1.0));
    double cosb  = std::cos(beta);
    double gamma = 0.0;
    if (std::abs(cosb) > 1e-6) {
        gamma = std::atan2(-ref_to_local(0, 1), ref_to_local(1, 1));
    }
    ptr->set_zrot_radians(gamma);
}

static void install_group(SD::element_ptr ptr, GroupParameters const& param) {
    ptr->set_aperture(param.aperture);
    ptr->set_surface(param.surface);

    ptr->set_front_optical_properties(param.optics_front);
    ptr->set_back_optical_properties(param.optics_back);
}

std::shared_ptr<SD::SimulationData> export_to_simdata(entt::registry& reg) {


    SD::SimulationData ret;

    auto param_ptr = reg.ctx().find<SD::SimulationParameters>();
    if (!param_ptr) return nullptr;

    ret.get_simulation_parameters() = *param_ptr;

    auto ray_source_ptr = reg.ctx().find<RaySourceResource>();
    if (!ray_source_ptr) return nullptr;

    ret.add_ray_source(ray_source_ptr->source);

    std::unordered_map<entt::entity, SD::element_ptr> entity_element_map;

    // Mirror all elements

    {
        auto view = reg.view<const ElementComponent>();
        for (auto const& [e] : view.each()) {
            SD::element_ptr ptr;

            if (reg.all_of<ChildrenComponent>(e)) {
                auto n = std::make_shared<SD::CompositeElement>();
                ptr    = n;
            } else {
                auto n = std::make_shared<SD::SingleElement>();
                ptr    = n;
            }

            entity_element_map[e] = ptr;
        }
    }

    {
        auto view = reg.view<const TransformComponent>();
        for (auto const& [e, tf] : view.each()) {
            install_transform(entity_element_map.at(e), tf);
        }
    }

    {
        auto view = reg.view<const IdentityComponent>();
        for (auto const& [e, tf] : view.each()) {
            entity_element_map.at(e)->set_name(tf.name.toStdString());
        }
    }

    {
        auto view = reg.view<const DisabledComponent>();
        for (auto const& [e] : view.each()) {
            entity_element_map.at(e)->disable();
        }
    }

    {
        auto view = reg.view<const GroupMemberComponent>();
        for (auto const& [e, gm] : view.each()) {

            // get group, we assume this is valid
            auto const& group = reg.get<GroupComponent>(gm.group).parameters;

            auto element = entity_element_map.at(e);

            if (auto ptr = reg.try_get<ChildrenComponent>(e); ptr) {
                if (!ptr->children.empty()) {
                    // it has children AND a group component. add a proxy that
                    // will only hold the geometry

                    auto composite =
                        dynamic_cast<SD::CompositeElement*>(element.get());
                    Q_ASSERT(composite);

                    auto n = std::make_shared<SD::SingleElement>();
                    composite->add_element(n);
                    install_group(n, *group);

                    continue;
                }
            }

            // has no children

            install_group(element, *group);
        }
    }

    {
        auto view = reg.view<const ChildrenComponent>();
        for (auto const& [e, children] : view.each()) {

            auto composite = dynamic_cast<SD::CompositeElement*>(
                entity_element_map.at(e).get());
            Q_ASSERT(composite);

            for (auto child : children.children) {

                auto iter = entity_element_map.find(child);

                if (iter == entity_element_map.end()) {
                    qCritical() << "Missing child element";
                    continue;
                }

                composite->add_element(iter->second);
            }
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
    if (!reg.valid(parent) or !reg.valid(child)) {
        qWarning() << "Invalid parent or child in set_parent";
        return;
    }

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

    emplace_patch<GroupComponent>(
        reg, group, [child](GroupComponent& c) { c.members.push_back(child); });
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

        return std::find(t.begin(), t.end(), tag) != t.end();
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

    // if (storage.empty()) { reg.reset(entt::to_integral(tag)); }
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
