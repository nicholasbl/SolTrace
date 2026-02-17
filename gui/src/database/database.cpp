#include "database.h"
#include "conversion.h"
#include "database/components.h"
#include "database/database_notification.h"
#include "utilities/math_utility.h"

#include "simulation_data_api.hpp"
#include "simdata_io.hpp"

#include <algorithm>
#include <cmath>
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL 1
#include <glm/gtx/io.hpp>

#include <QDebug>

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
struct std::hash<db::GroupParameterComponent> {
    std::size_t operator()(db::GroupParameterComponent const& a) const {
        size_t seed = 0;

        if (a.aperture) hash_combine(seed, *a.aperture);
        if (a.surface) hash_combine(seed, *a.surface);
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
        hash_combine(seed, aa->x_length());
        hash_combine(seed, aa->y_length());
        hash_combine(seed, aa->x_coord());
        hash_combine(seed, aa->y_coord());
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


TransformComponent extract_tf(SD::Element const& e) {
    auto pos = e.get_origin_ref();

    auto aim = e.get_aim_vector_ref();

    aim = glm::normalize(aim - pos);

    auto quat = dir_roll_to_quat(aim, e.get_zrot_radians());

    // TODO FIX
    auto quat2 = glm::quat_cast(e.get_local_to_reference());

    // qDebug() << quat << quat2;

    return TransformComponent { .position = pos, .rotation = quat2 };
}


TransformComponent extract_tf_stage(SD::Element const& e) {
    auto pos = e.get_origin_stage();

    auto aim = e.get_aim_vector_stage();

    aim = glm::normalize(aim - pos);

    auto quat = dir_roll_to_quat(aim, e.get_zrot_radians());

    auto quat2 = glm::quat_cast(e.get_local_to_reference());

    // qDebug() << quat << quat2;

    return TransformComponent { .position = pos, .rotation = quat2 };
}


// =============================================================================

Database::Database(QObject* p)
    : QObject(p),
      m_registry(),
      identity(m_registry),
      transform(m_registry),
      invisible(m_registry),
      parent(m_registry),
      tag_root(m_registry),
      group_root(m_registry),
      group_parameters(m_registry),
      children(m_registry),
      group_membership(m_registry),
      tag_membership(m_registry) { }

// =============================================================================

static void
import_optics(Database&                                          reg,
              entt::entity                                       entity,
              SD::Element const&                                 item,
              std::unordered_map<GroupParameterComponent, entt::entity>& groups,
              size_t& group_counter) {

    auto& registry = reg.as_registry();

    if (!item.get_front_optical_properties() and
        !item.get_back_optical_properties() and !item.get_aperture() and
        !item.get_surface()) {
        // does not have geometry.
        qDebug() << "Skipping optics on" << entt::to_integral(entity);
        return;
    }

    if (!item.get_front_optical_properties() or
        !item.get_back_optical_properties() or !item.get_aperture() or
        !item.get_surface()) {

        emplace_patch<ImportErrorComponent>(reg, entity, [](auto& c) {
            c.reason += "Missing aperture and surface";
        });
        qWarning() << "Entity" << entt::to_integral(entity)
                   << "missing surface properties";
        return;
    }

    auto local = GroupParameterComponent {
        .aperture     = item.get_aperture(),
        .surface      = item.get_surface(),
        .optics_front = *item.get_front_optical_properties(),
        .optics_back  = *item.get_back_optical_properties(),
    };

    if (auto group = groups.find(local); group != groups.end()) {
        // we have such a group

        reg.assign_group(entity, group->second);

    } else {
        // no such group

        auto new_group_params = local;

        auto new_group = GroupComponent {};

        auto group_entity = reg.create();

        registry.emplace<GroupComponent>(group_entity, new_group);
        registry.emplace<GroupParameterComponent>(group_entity,
                                                  new_group_params);
        registry.emplace<IdentityComponent>(
            group_entity,
            IdentityComponent {
                .name = QString("Group %1").arg(group_counter),
            });

        group_counter++;

        groups.try_emplace(local, group_entity);

        reg.assign_group(entity, group_entity);
    }
}

struct StageComponent {
    TransformComponent stage_tf;
    TransformComponent this_in_stage;
};

void Database::import(SD::SimulationData& data) {

    // Assuming we are not re-using registries, which we are not for the moment
    auto imported_tag = create_tag("imported");

    // we use pointers as element IDs are scoped to global AND composite
    std::unordered_map<SD::Element*, entt::entity> element_to_entity;

    auto get_or_create_entity = [&](SD::Element* ptr) -> entt::entity {
        if (!ptr) return entt::null;

        if (auto eiter = element_to_entity.find(ptr);
            eiter != element_to_entity.end()) {
            return eiter->second;
        } else {
            auto ent  = m_registry.create();
            element_to_entity[ptr] = ent;

            m_registry.emplace<ElementComponent>(ent);

            assign_tag(ent, imported_tag);

            return ent;
        }
    };

    // MAP MAY NOT BE RE-USED!
    std::unordered_map<GroupParameterComponent, entt::entity> groups;

    size_t group_counter = 0;

    for (auto iter = data.get_iterator(); !data.is_at_end(iter); ++iter) {
        auto const& element = *(iter->second);

        if (element.is_stage()) {
            // we want to avoid materializing stage elements
            auto c = std::dynamic_pointer_cast<SD::StageElement>(iter->second);

            TransformComponent stage_tf = extract_tf(element);

            qDebug() << stage_tf.position << stage_tf.rotation;

            for (auto iter = c->get_const_iterator(); !c->is_at_end(iter);
                 ++iter) {

                auto child_ent = get_or_create_entity(iter->second.get());

                auto child_tf = extract_tf(*iter->second);

                qDebug() << (child_tf.position -
                             extract_tf_stage(*iter->second).position)
                                .length();

                m_registry.emplace_or_replace<StageComponent>(
                    child_ent,
                    StageComponent {
                        .stage_tf      = stage_tf,
                        .this_in_stage = extract_tf_stage(*iter->second),
                    });
            }

            // we do NOT add these as children, we instead make them globals
            continue;
        }

        entt::entity ent = get_or_create_entity(iter->second.get());

        if (!m_registry.valid(ent)) {
            qWarning() << "Unable to mirror element" << &element;
        }

        // auto position = element.get_origin_ref();

        // auto rotation = convert(element.get_local_to_reference());

        m_registry.emplace<TransformComponent>(ent, extract_tf(element));

        if (!element.get_name().empty()) {
            m_registry.emplace<IdentityComponent>(
                ent, QString::fromStdString(element.get_name()));
        }

        if (!element.is_enabled()) {
            m_registry.emplace<DisabledComponent>(ent);
        }

        if (element.is_composite()) {
            auto& c = *static_cast<SD::CompositeElement const*>(&element);

            for (auto iter = c.get_const_iterator(); !c.is_at_end(iter);
                 ++iter) {

                auto child_ent = get_or_create_entity(iter->second.get());

                if (m_registry.any_of<ChildOfComponent>(child_ent)) {
                    // Uh oh. We should not have multiple parents!
                    throw std::runtime_error("Multiple parents for element");
                }

                set_parent(child_ent, ent);
            }
        }

        import_optics(*this, ent, element, groups, group_counter);
    }

    {
        // burn in globals from a stage

        auto view = m_registry.view<StageComponent const>();

        for (auto const& [e, stage] : view.each()) {
            // qDebug() << entt::to_integral(e);

            auto new_pos =
                stage.stage_tf.position +
                stage.stage_tf.rotation * stage.this_in_stage.position;
            auto new_rot =
                stage.stage_tf.rotation * stage.this_in_stage.rotation;

            // qDebug() << new_pos << new_rot;
            // qDebug() << stage.this_in_stage.position
            //          << stage.this_in_stage.rotation;

            m_registry.emplace_or_replace<TransformComponent>(
                e,
                TransformComponent {
                    .position = new_pos,
                    .rotation = new_rot,
                });
        }

        m_registry.clear<StageComponent>();
    }


    m_registry.ctx().emplace<RaySourceResource>(RaySourceResource {
        .source = data.get_ray_source(),
    });

    m_registry.ctx().emplace<SD::SimulationParameters>(
        data.get_simulation_parameters());

    qInfo() << "Imported" << this->m_registry.view<ElementComponent>()->size()
            << "elements";

    qInfo() << "Imported" << this->m_registry.view<GroupComponent>()->size()
            << "groups";
}

// TODO: remove once we have sorted the vector aim thing
static void install_transform(SD::element_ptr           ptr,
                              TransformComponent const& tf_comp) {

    glm::dvec3 aim  = {};
    double     roll = {};
    quat_to_dir_roll(tf_comp.rotation, aim, roll);

    auto origin = tf_comp.position;
    ptr->set_origin(origin.x, origin.y, origin.z);

    aim = (origin + aim * 100.0);

    ptr->set_aim_vector(aim.x, aim.y, aim.z);

    ptr->set_zrot_radians(roll);
}

static void install_group(SD::element_ptr ptr, GroupParameterComponent const& param) {
    ptr->set_aperture(param.aperture);
    ptr->set_surface(param.surface);

    ptr->set_front_optical_properties(param.optics_front);
    ptr->set_back_optical_properties(param.optics_back);
}

std::shared_ptr<SD::SimulationData> Database::export_to_simdata() {
    SD::SimulationData ret;

    auto param_ptr = m_registry.ctx().find<SD::SimulationParameters>();
    if (!param_ptr) return nullptr;

    ret.get_simulation_parameters() = *param_ptr;

    auto ray_source_ptr = m_registry.ctx().find<RaySourceResource>();
    if (!ray_source_ptr) return nullptr;

    ret.add_ray_source(ray_source_ptr->source);

    std::unordered_map<entt::entity, SD::element_ptr> entity_element_map;

    // Mirror all elements

    {
        auto view = m_registry.view<const ElementComponent>();
        for (auto const& [e] : view.each()) {
            SD::element_ptr ptr;

            if (children_of(e).size()) {
                auto n = std::make_shared<SD::CompositeElement>();
                ptr    = n;
            } else {
                auto n = std::make_shared<SD::SingleElement>();
                ptr    = n;
            }

            entity_element_map[e] = ptr;
        }
    }

    qDebug() << Q_FUNC_INFO << entity_element_map.size();
    qDebug() << Q_FUNC_INFO << ret.get_number_of_elements();

    {
        auto view = m_registry.view<const TransformComponent>();
        for (auto const& [e, tf] : view.each()) {
            install_transform(entity_element_map.at(e), tf);
        }
    }

    {
        auto view =
            m_registry.view<const ElementComponent, const IdentityComponent>();
        for (auto const& [e, ident] : view.each()) {
            entity_element_map.at(e)->set_name(ident.name.toStdString());
        }
    }

    {
        auto view = m_registry.view<const DisabledComponent>();
        for (auto const& [e] : view.each()) {
            entity_element_map.at(e)->disable();
        }
    }

    {
        auto view = m_registry.view<const GroupMemberComponent>();
        for (auto const& [e, gm] : view.each()) {

            // get group, we assume this is valid
            auto const& group =
                m_registry.get<GroupParameterComponent>(gm.group);

            auto element = entity_element_map.at(e);

            if (auto ptr = m_registry.try_get<ChildrenComponent>(e); ptr) {
                if (!ptr->children.empty()) {
                    // it has children AND a group component. add a proxy that
                    // will only hold the geometry

                    auto composite =
                        dynamic_cast<SD::CompositeElement*>(element.get());
                    Q_ASSERT(composite);

                    auto n = std::make_shared<SD::SingleElement>();
                    composite->add_element(n);
                    install_group(n, group);

                    continue;
                }
            }

            // has no children

            install_group(element, group);
        }
    }

    {
        auto view = m_registry.view<const ChildrenComponent>();
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

                if (iter->second->is_composite()) {
                    qCritical()
                        << "Composite child under non-stage composite is not supported";
                    continue;
                }

                auto ret = composite->add_element(iter->second);

                if (ret < 0) { qCritical() << "Add failed"; }
            }
        }
    }

    // Install all elements into the sim engine
    for (auto const& iter : entity_element_map) {
        if (m_registry.any_of<ChildOfComponent>(iter.first)) {
            // Only add top-level elements; composites will add subelements.
            continue;
        }
        auto const& ptr = iter.second;
        // qDebug() << entt::to_integral(iter.first) << ptr->is_single()
        //          << ptr->is_composite() << ptr->is_stage();
        try {
            ret.add_element(iter.second);
        } catch (std::exception const& e) {
            qCritical() << "Unable to export entity"
                        << entt::to_integral(iter.first);

            return nullptr;
        }
    }


    // try {
    //     SD::write_json_file(ret, "/tmp/soltrace_export.json");
    // } catch (std::exception const& e) {
    //     qWarning() << "Failed to write export JSON dump:" << e.what();
    // }

    return std::make_shared<SD::SimulationData>(std::move(ret));
}

entt::entity Database::create() {
    return m_registry.create();
}

bool Database::valid(entt::entity e) const {
    return m_registry.valid(e);
}

void Database::unset_parent(entt::entity child) {

    auto child_comp = m_registry.try_get<ChildOfComponent>(child);

    if (!child_comp) return;

    auto has_parent_comp =
        m_registry.all_of<ChildrenComponent>(child_comp->parent);

    if (!has_parent_comp) {
        m_registry.erase<ChildOfComponent>(child);
        return;
    }

    m_registry.erase<ChildOfComponent>(child);

    m_registry.patch<ChildrenComponent>(
        child_comp->parent,
        [child](ChildrenComponent& c) { erase(c.children, child); });
}

void Database::set_parent(entt::entity child, entt::entity parent) {
    if (!m_registry.valid(parent) or !m_registry.valid(child)) {
        qWarning() << "Invalid parent or child in set_parent";
        return;
    }

    // remove existing parent, if any
    unset_parent(child);

    if (!m_registry.all_of<ChildrenComponent>(parent)) {
        // if the parent doesnt have a child component, insert this as our first
        // child
        m_registry.emplace<ChildrenComponent>(parent,
                                              ChildrenComponent {
                                                  .children = { child },
                                              });
    } else {
        // it has a child component, add to it
        m_registry.patch<ChildrenComponent>(
            parent,
            [child](ChildrenComponent& a) { a.children.push_back(child); });
    }

    // set the child's parent
    m_registry.emplace<ChildOfComponent>(child,
                                         ChildOfComponent { .parent = parent });
}

std::span<entt::entity const> Database::children_of(entt::entity parent) const {
    auto parent_comp = m_registry.try_get<ChildrenComponent>(parent);

    if (!parent_comp) { return {}; }

    return parent_comp->children;
}

entt::entity Database::parent_of(entt::entity child) const {
    if (!m_registry.valid(child)) return entt::null;
    auto* ptr = m_registry.try_get<ChildOfComponent>(child);

    if (!ptr) return entt::null;

    return ptr->parent;
}

void Database::unset_group(entt::entity child) {
    // is this a member of a group?
    auto child_comp = m_registry.try_get<GroupMemberComponent>(child);

    if (!child_comp) return;

    // Check if the group parent exists
    auto parent_comp = m_registry.try_get<GroupComponent>(child_comp->group);

    // remove us from a member of the group
    m_registry.erase<GroupMemberComponent>(child);

    if (!parent_comp) { return; }

    // remove us from the parent
    erase(parent_comp->members, child);
}

void Database::assign_group(entt::entity child, entt::entity group) {
    if (!valid(child) or !valid(group)) return;

    if (!m_registry.all_of<GroupComponent>(group)) return;

    unset_group(child);

    m_registry.emplace_or_replace<GroupMemberComponent>(
        child, GroupMemberComponent { .group = group });

    ::db::emplace_patch<GroupComponent>(
        m_registry, group, [child](GroupComponent& c) {
            c.members.push_back(child);
        });
}

SD::ray_source_ptr Database::get_ray_source() const {
    if (auto ptr = m_registry.ctx().find<RaySourceResource>(); ptr) {
        return ptr->source;
    }

    return {};
}

SD::SimulationParameters const& Database::get_sim_params() const {
    if (auto ptr = m_registry.ctx().find<SD::SimulationParameters>(); ptr) {
        return *ptr;
    }

    throw std::runtime_error("missing simulation parameters");
}

TransformComponent Database::global_transform(entt::entity item) const {
    TransformComponent out;
    out.position = glm::dvec3 { 0.0 };
    out.rotation = glm::dquat { 1.0, 0.0, 0.0, 0.0 };

    entt::entity current = item;

    while (current != entt::null) {
        if (auto* t = m_registry.try_get<TransformComponent>(current)) {
            // apply current local, then whatever accumulated so
            // far.
            out.position = t->position + t->rotation * out.position;
            out.rotation = t->rotation * out.rotation;
        }

        if (auto* child_of = m_registry.try_get<ChildOfComponent>(current)) {
            current = child_of->parent;
        } else {
            break;
        }
    }

    return out;
}

entt::entity Database::create_tag(QString name) {
    auto ret = m_registry.create();

    m_registry.emplace<TagComponent>(ret);
    m_registry.emplace<IdentityComponent>(ret,
                                          IdentityComponent { .name = name });

    return ret;
}

bool Database::is_tagged(entt::entity item, entt::entity tag) const {
    if (auto* ptr = m_registry.try_get<TagMembershipComponent>(item); ptr) {
        auto& t = ptr->tags;

        return std::find(t.begin(), t.end(), tag) != t.end();
    }
    return false;
}

void Database::assign_tag(entt::entity item, entt::entity tag) {
    if (!m_registry.all_of<TagComponent>(tag)) { return; }

    if (is_tagged(item, tag)) return;

    auto& storage =
        m_registry.storage<ATagMemberComponent>(entt::to_integral(tag));

    storage.emplace(item);

    ::db::emplace_patch<TagMembershipComponent>(
        m_registry, item, [tag](TagMembershipComponent& tc) {
            tc.tags.push_back(tag);
        });
}

void Database::unassign_tag(entt::entity item, entt::entity tag) {
    if (!m_registry.all_of<TagComponent>(tag)) { return; }

    if (!m_registry.all_of<TagMembershipComponent>(item)) { return; }

    m_registry.patch<TagMembershipComponent>(
        item, [tag](TagMembershipComponent& tc) { erase(tc.tags, tag); });

    auto& storage =
        m_registry.storage<ATagMemberComponent>(entt::to_integral(tag));

    if (storage.contains(item)) { storage.erase(item); }

    // if (storage.empty()) { reg.reset(entt::to_integral(tag)); }
}

void Database::delete_tag(entt::entity tag) {
    auto& storage =
        m_registry.storage<ATagMemberComponent>(entt::to_integral(tag));

    for (auto x : storage) {
        erase(m_registry.get<TagMembershipComponent>(x).tags, tag);
    }

    m_registry.reset(entt::to_integral(tag));

    m_registry.destroy(tag);
}

std::span<entt::entity const> Database::tags_for(entt::entity item) const {
    if (auto ptr = m_registry.try_get<TagMembershipComponent>(item); ptr) {
        return ptr->tags;
    }

    return {};
}

QString Database::name_of(entt::entity item) const {
    if (!m_registry.valid(item)) return {};

    if (auto ptr = m_registry.try_get<IdentityComponent>(item); ptr) {
        return ptr->name;
    }

    return QString("Entity %1").arg(entt::to_integral(item));
}


entt::entity Database::add_group(QString               new_name,
                                 QVector<entt::entity> members,
                                 entt::entity          clone_from) {
    auto set = std::unordered_set(members.begin(), members.end());


    GroupParameterComponent params;

    if (m_registry.valid(clone_from) and
        m_registry.all_of<GroupParameterComponent>(clone_from)) {

        auto& other_p = m_registry.get<GroupParameterComponent>(clone_from);

        // horrible, but it works. library classes don't all have clone()
        nlohmann::ordered_json node;

        other_p.surface->write_json(node);

        params.aperture = other_p.aperture->make_copy();
        params.surface  = SD::make_surface_from_json(node);
    } else {
        params.aperture = SD::make_aperture<SD::Circle>(1.0);
        params.surface  = SolTrace::Data::make_surface_from_type(
            SolTrace::Data::SurfaceType::FLAT, { 1.0, 1.0 });
    }


    for (auto mem : members) {
        this->unset_group(mem);
    }

    auto ent = m_registry.create();
    m_registry.emplace<GroupComponent>(
        ent,
        GroupComponent {
            .members = QVector<entt::entity>(set.begin(), set.end()),
        });
    m_registry.emplace<GroupParameterComponent>(ent, params);


    m_registry.emplace<IdentityComponent>(
        ent, IdentityComponent { .name = new_name });

    for (auto child : set) {
        m_registry.emplace_or_replace<GroupMemberComponent>(
            child, GroupMemberComponent { .group = ent });
    }

    return ent;
}

void Database::delete_group(entt::entity to_delete, entt::entity move_to) {
    if (to_delete == move_to) {
        qWarning()
            << "Trying to delete a group and move members to the same group!";
        return;
    }

    // if not a group, bail
    if (!m_registry.all_of<GroupComponent>(to_delete)) { return; }

    // steal current member list
    auto members = std::move(m_registry.get<GroupComponent>(to_delete).members);

    // destroy current group entity
    m_registry.destroy(to_delete);


    if (m_registry.valid(move_to) and
        m_registry.all_of<GroupComponent>(move_to)) {
        // moving to valid target

        // reset member list membership
        for (auto child : members) {
            m_registry.emplace_or_replace<GroupMemberComponent>(
                child, GroupMemberComponent { .group = move_to });
        }

        m_registry.patch<GroupComponent>(move_to, [&](GroupComponent& a) {
            a.members.append(members.begin(), members.end());
        });
    } else {
        // invalid target. clear

        m_registry.remove<GroupMemberComponent>(members.begin(), members.end());
    }
}

} // namespace db
