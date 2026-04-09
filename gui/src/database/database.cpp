#include "database.h"
#include "conversion.h"
#include "database/components.h"
#include "database/database_notification.h"
#include "utilities/math_utility.h"

#include "simdata_io.hpp"
#include "simulation_data_api.hpp"

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
struct std::hash<db::MaterialComponent> {
    std::size_t operator()(db::MaterialComponent const& a) const {
        size_t seed = 0;

        hash_combine(seed, a.optics_front);
        hash_combine(seed, a.optics_back);

        return seed;
    }
};

template <>
struct std::hash<db::GeometryComponent> {
    std::size_t operator()(db::GeometryComponent const& a) const {
        size_t seed = 0;

        if (a.aperture) hash_combine(seed, *a.aperture);
        if (a.surface) hash_combine(seed, *a.surface);

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
    case SolTrace::Data::SINGLE_AXIS_CURVATURE_SECTION: break;
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
    case SolTrace::Data::APERTURE_UNKNOWN: break;
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
    case SolTrace::Data::FLAT: break;
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
    case SolTrace::Data::SURFACE_UNKNOWN: break;
    }

    return seed;
}

namespace db {

TransformComponent extract_tf(SD::Element const& e) {
    auto pos = e.get_origin_ref();

    auto aim = e.get_aim_vector_ref();

    aim = glm::normalize(aim - pos);

    auto quat = dir_roll_to_quat(aim, e.get_zrot_radians());

    // TODO: FIX
    // I dont understand why the above is incorrect and the below is right
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

static GlobalTransformComponent
compute_global_without_local_transform(entt::registry& reg,
                                       entt::entity    entity) {
    if (auto* child_of = reg.try_get<ChildOfComponent>(entity)) {
        return GlobalTransformComponent::compute_for(reg, child_of->parent);
    }

    return GlobalTransformComponent {
        .position = glm::dvec3 { 0.0 },
        .rotation = glm::dquat { 1.0, 0.0, 0.0, 0.0 },
    };
}

static void update_global_transform_subtree(
    entt::registry&                         reg,
    entt::entity                            entity,
    std::optional<GlobalTransformComponent> root_transform = std::nullopt) {
    auto global = root_transform.value_or(
        GlobalTransformComponent::compute_for(reg, entity));

    reg.emplace_or_replace<GlobalTransformComponent>(entity, global);

    auto* ptr = reg.try_get<ChildrenComponent>(entity);
    if (!ptr) return;

    // Copy before recursion in case a callback mutates the child list.
    auto cpy = ptr->children;

    for (auto e : std::as_const(cpy)) {
        update_global_transform_subtree(reg, e);
    }
}

static void tf_change_callback(entt::registry& reg, entt::entity entity) {
    update_global_transform_subtree(reg, entity);
}

static void tf_destroy_callback(entt::registry& reg, entt::entity entity) {
    update_global_transform_subtree(
        reg, entity, compute_global_without_local_transform(reg, entity));
}

Database::Database(QObject* p)
    : QObject(p),
      m_registry(),
      identity(m_registry),
      transform(m_registry),
      global_transform(m_registry),
      invisible(m_registry),
      parent(m_registry),
      tag_root(m_registry),
      material_root(m_registry),
      material_parameters(m_registry),
      material_group_membership(m_registry),
      geometry_root(m_registry),
      geometry_parameters(m_registry),
      geometry_group_membership(m_registry),
      children(m_registry),
      tag_membership(m_registry),
      selected(m_registry),
      color(m_registry) {

    m_registry.on_construct<TransformComponent>()
        .template connect<&tf_change_callback>();
    m_registry.on_update<TransformComponent>()
        .template connect<&tf_change_callback>();
    m_registry.on_destroy<TransformComponent>()
        .template connect<&tf_destroy_callback>();
}

// =============================================================================

struct EntityMapper {
    entt::registry&                                new_registry;
    std::unordered_map<entt::entity, entt::entity> old_to_new_map;

    entt::entity operator()(entt::entity e) {
        auto item = old_to_new_map.find(e);

        if (item == old_to_new_map.end()) {
            auto new_e        = new_registry.create();
            old_to_new_map[e] = new_e;
            return e;
        }

        return item->second;
    }
};

template <class T>
void copy_marker_component(entt::registry const& from,
                           EntityMapper&         mapper,
                           entt::registry&       to) {
    for (auto [e] : from.view<T>().each()) {
        to.emplace<T>(mapper(e));
    }
}

template <class T>
void copy_plain_component(entt::registry const& from,
                          EntityMapper&         mapper,
                          entt::registry&       to) {
    for (auto [e, c] : from.view<T>().each()) {
        to.emplace<T>(mapper(e), c);
    }
}

template <class T>
void copy_nested_component(entt::registry const& from,
                           EntityMapper&         mapper,
                           entt::registry&       to) {
    for (auto [e, c] : from.view<T>().each()) {
        T component_copy = c;
        component_copy.remap_entities(mapper);
        to.emplace<T>(mapper(e), component_copy);
    }
}

template <class T>
void copy_resource(entt::registry const& from, entt::registry& to) {

    to.ctx().emplace<T>(from.ctx().get<T>().clone());
}

Database* Database::clone(QObject* p) const {
    auto ret = new Database(p);

    EntityMapper mapper {
        .new_registry   = ret->m_registry,
        .old_to_new_map = {},
    };

    copy_marker_component<InvisibleComponent>(
        this->m_registry, mapper, ret->m_registry);

    copy_marker_component<DisabledComponent>(
        this->m_registry, mapper, ret->m_registry);

    copy_plain_component<IdentityComponent>(
        this->m_registry, mapper, ret->m_registry);


    copy_marker_component<ElementComponent>(
        this->m_registry, mapper, ret->m_registry);


    copy_nested_component<ChildOfComponent>(
        this->m_registry, mapper, ret->m_registry);

    copy_nested_component<ChildrenComponent>(
        this->m_registry, mapper, ret->m_registry);


    copy_plain_component<TransformComponent>(
        this->m_registry, mapper, ret->m_registry);

    copy_plain_component<GlobalTransformComponent>(
        this->m_registry, mapper, ret->m_registry);


    copy_resource<RaySourceResource>(this->m_registry, ret->m_registry);


    copy_plain_component<MaterialComponent>(
        this->m_registry, mapper, ret->m_registry);

    copy_nested_component<MaterialGroupComponent>(
        this->m_registry, mapper, ret->m_registry);

    copy_nested_component<MaterialGroupMemberComponent>(
        this->m_registry, mapper, ret->m_registry);

    // GeometryComponent
    {
        for (auto [e, c] : m_registry.view<GeometryComponent>().each()) {
            auto local = c.clone();
            ret->m_registry.emplace<GeometryComponent>(mapper(e), local);
        }
    }

    copy_nested_component<GeometryGroupComponent>(
        this->m_registry, mapper, ret->m_registry);

    copy_nested_component<GeometryGroupMemberComponent>(
        this->m_registry, mapper, ret->m_registry);

    copy_marker_component<TagComponent>(
        this->m_registry, mapper, ret->m_registry);

    // ATagMemberComponent
    {
        // we need to know what all tags are out there

        auto all_tags = QSet<entt::entity>();

        for (auto [e, c] :
             this->m_registry.view<TagMembershipComponent>().each()) {
            for (auto const& t : c.tags) {
                all_tags.insert(t);
            }
        }

        for (auto tag_ent : std::as_const(all_tags)) {
            auto from_storage = m_registry.storage<ATagMemberComponent>(
                entt::to_integral(tag_ent));

            auto& to_storage = ret->m_registry.storage<ATagMemberComponent>(
                entt::to_integral(mapper(tag_ent)));

            for (auto element : *from_storage) {
                to_storage.emplace(mapper(element));
            }
        }
    }

    copy_nested_component<TagMembershipComponent>(
        this->m_registry, mapper, ret->m_registry);

    copy_plain_component<ImportErrorComponent>(
        this->m_registry, mapper, ret->m_registry);

    copy_marker_component<SelectedComponent>(
        this->m_registry, mapper, ret->m_registry);

    copy_plain_component<ColorComponent>(
        this->m_registry, mapper, ret->m_registry);

    copy_plain_component<HasFluxMapComponent>(
        this->m_registry, mapper, ret->m_registry);

    return ret;
}

// =============================================================================

static void import_optics(
    Database&                                            reg,
    entt::entity                                         entity,
    SD::Element const&                                   item,
    std::unordered_map<MaterialComponent, entt::entity>& material_groups,
    std::unordered_map<GeometryComponent, entt::entity>& geometry_groups,
    size_t&                                              group_counter) {

    auto& registry = reg.as_registry();

    // If it has nothing, dont import
    if (!item.get_front_optical_properties() and
        !item.get_back_optical_properties() and !item.get_aperture() and
        !item.get_surface()) {
        // does not have geometry.
        qDebug() << "Skipping optics on" << entt::to_integral(entity);
        return;
    }

    // It should have all or nothing
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

    auto local_mat = MaterialComponent {
        .optics_front = *item.get_front_optical_properties(),
        .optics_back  = *item.get_back_optical_properties(),
    };

    auto local_geom = GeometryComponent {
        .aperture = item.get_aperture(),
        .surface  = item.get_surface(),
    };

    if (auto group = material_groups.find(local_mat);
        group != material_groups.end()) {
        // we have such a group

        reg.assign_material(entity, group->second);

    } else {
        // no such group

        auto new_group_params = local_mat;

        auto new_group = MaterialGroupComponent {};

        auto group_entity = reg.create();

        registry.emplace<MaterialGroupComponent>(group_entity, new_group);
        registry.emplace<MaterialComponent>(group_entity, new_group_params);
        registry.emplace<IdentityComponent>(
            group_entity,
            IdentityComponent {
                .name = QString("Material %1").arg(group_counter),
            });

        group_counter++;

        material_groups.try_emplace(local_mat, group_entity);

        reg.assign_material(entity, group_entity);
    }

    if (auto group = geometry_groups.find(local_geom);
        group != geometry_groups.end()) {
        // we have such a group

        reg.assign_geometry(entity, group->second);

    } else {
        // no such group

        auto new_group_params = local_geom;

        auto new_group = GeometryGroupComponent {};

        auto group_entity = reg.create();

        registry.emplace<GeometryGroupComponent>(group_entity, new_group);
        registry.emplace<GeometryComponent>(group_entity, new_group_params);
        registry.emplace<IdentityComponent>(
            group_entity,
            IdentityComponent {
                .name = QString("Geometry %1").arg(group_counter),
            });

        group_counter++;

        geometry_groups.try_emplace(local_geom, group_entity);

        reg.assign_geometry(entity, group_entity);
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
            auto ent               = m_registry.create();
            element_to_entity[ptr] = ent;

            m_registry.emplace<ElementComponent>(ent);

            assign_tag(ent, imported_tag);

            return ent;
        }
    };

    // MAP MAY NOT BE RE-USED!
    std::unordered_map<MaterialComponent, entt::entity> material_groups;
    std::unordered_map<GeometryComponent, entt::entity> geometry_groups;

    size_t group_counter = 0;

    for (auto iter = data.get_iterator(); !data.is_at_end(iter); ++iter) {
        auto const& element = *(iter->second);

        // if (element.get_name() == "0") {
        //     qDebug() << "CHECK 0" << element.is_composite()
        //              << element.is_single() << element.get_surface().get()
        //              << element.get_aperture().get();
        // }

        if (element.is_stage()) {
            // we want to avoid materializing stage elements
            auto c = std::dynamic_pointer_cast<SD::StageElement>(iter->second);

            TransformComponent stage_tf = extract_tf(element);

            for (auto iter = c->get_const_iterator(); !c->is_at_end(iter);
                 ++iter) {

                auto child_ent = get_or_create_entity(iter->second.get());

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

        import_optics(*this,
                      ent,
                      element,
                      material_groups,
                      geometry_groups,
                      group_counter);
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

    qInfo() << "Imported"
            << this->m_registry.view<MaterialGroupComponent>()->size()
            << "materials";
    qInfo() << "Imported"
            << this->m_registry.view<GeometryGroupComponent>()->size()
            << "geometries";
}

Database::operator entt::registry&() {
    return m_registry;
}

Database::operator const entt::registry&() const {
    return m_registry;
}

entt::registry& Database::as_registry() {
    return m_registry;
}

const entt::registry& Database::as_registry() const {
    return m_registry;
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

static void install_group(SD::element_ptr          ptr,
                          MaterialComponent const& param,
                          GeometryComponent const& geom_param) {
    ptr->set_aperture(geom_param.aperture);
    ptr->set_surface(geom_param.surface);

    ptr->set_front_optical_properties(param.optics_front);
    ptr->set_back_optical_properties(param.optics_back);
}

std::shared_ptr<DatabaseExport> Database::export_to_simdata() {
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

            ptr->set_name(name_of(e).toStdString());

            entity_element_map[e] = ptr;
        }
    }

    // qDebug() << Q_FUNC_INFO << entity_element_map.size();
    // qDebug() << Q_FUNC_INFO << ret.get_number_of_elements();

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
        auto view = m_registry.view<const MaterialGroupMemberComponent,
                                    const GeometryGroupMemberComponent>();
        for (auto const& [e, mat, geom] : view.each()) {

            // get group, we assume this is valid
            auto const& mat_group =
                m_registry.get<MaterialComponent>(mat.group);
            auto const& geom_group =
                m_registry.get<GeometryComponent>(geom.group);

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
                    install_group(n, mat_group, geom_group);

                    continue;
                }
            }

            // has no children

            // if (element->get_name() == "0") {
            //     qDebug() << "Install" << entt::to_integral(e)
            //              << element->get_name() <<
            //              entt::to_integral(mat.group)
            //              << entt::to_integral(geom.group);
            // }


            install_group(element, mat_group, geom_group);
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
                    qCritical() << "Composite child under non-stage composite "
                                   "is not supported";
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
                        << entt::to_integral(iter.first)
                        << iter.second->get_name() << e.what();

            return nullptr;
        }
    }

    // try {
    //     SD::write_json_file(ret, "/tmp/soltrace_export.json");
    // } catch (std::exception const& e) {
    //     qWarning() << "Failed to write export JSON dump:" << e.what();
    // }

    std::unordered_map<SD::element_id, entt::entity> entity_rev_map;

    for (auto iter = entity_element_map.begin();
         iter != entity_element_map.end();
         ++iter) {
        entity_rev_map[iter->second->get_id()] = iter->first;
    }

    DatabaseExport export_ret;
    export_ret.data = std::make_shared<SD::SimulationData>(std::move(ret));
    export_ret.element_map = std::move(entity_rev_map);

    return std::make_shared<DatabaseExport>(std::move(export_ret));
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

    auto const parent = child_comp->parent;

    auto has_parent_comp = m_registry.all_of<ChildrenComponent>(parent);

    if (!has_parent_comp) {
        m_registry.erase<ChildOfComponent>(child);
        update_global_transform_subtree(m_registry, child);
        return;
    }

    m_registry.erase<ChildOfComponent>(child);

    m_registry.patch<ChildrenComponent>(
        parent, [child](ChildrenComponent& c) { erase(c.children, child); });

    update_global_transform_subtree(m_registry, child);
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

    update_global_transform_subtree(m_registry, child);
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

void Database::remove_material(entt::entity child) {
    // is this a member of a group?

    auto child_comp = m_registry.try_get<MaterialGroupMemberComponent>(child);

    if (!child_comp) return;

    // Check if the group parent exists
    auto parent_comp =
        m_registry.try_get<MaterialGroupComponent>(child_comp->group);

    // remove us from a member of the group
    m_registry.erase<MaterialGroupMemberComponent>(child);

    if (!parent_comp) { return; }

    // remove us from the parent

    m_registry.patch<MaterialGroupComponent>(
        child_comp->group,
        [child](MaterialGroupComponent& comp) { erase(comp.members, child); });
}

void Database::assign_material(entt::entity child, entt::entity group) {
    if (!valid(child) or !valid(group)) return;

    if (!m_registry.all_of<MaterialGroupComponent>(group)) return;

    remove_material(child);

    m_registry.emplace_or_replace<MaterialGroupMemberComponent>(
        child, MaterialGroupMemberComponent { .group = group });

    ::db::emplace_patch<MaterialGroupComponent>(
        m_registry, group, [child](MaterialGroupComponent& c) {
            c.members.push_back(child);
        });
}

void Database::remove_geometry(entt::entity child) {
    // is this a member of a group?
    auto child_comp = m_registry.try_get<GeometryGroupMemberComponent>(child);

    if (!child_comp) return;

    // Check if the group parent exists
    auto parent_comp =
        m_registry.try_get<GeometryGroupComponent>(child_comp->group);

    // remove us from a member of the group
    m_registry.erase<GeometryGroupMemberComponent>(child);

    if (!parent_comp) { return; }

    // remove us from the parent
    m_registry.patch<GeometryGroupComponent>(
        child_comp->group,
        [child](GeometryGroupComponent& comp) { erase(comp.members, child); });
}

void Database::assign_geometry(entt::entity child, entt::entity group) {
    if (!valid(child) or !valid(group)) return;

    if (!m_registry.all_of<GeometryGroupComponent>(group)) return;

    remove_geometry(child);

    m_registry.emplace_or_replace<GeometryGroupMemberComponent>(
        child, GeometryGroupMemberComponent { .group = group });

    ::db::emplace_patch<GeometryGroupComponent>(
        m_registry, group, [child](GeometryGroupComponent& c) {
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

        m_registry.patch<TagMembershipComponent>(
            x, [tag](TagMembershipComponent& comp) { erase(comp.tags, tag); });
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

QString Database::name_of(Entity item) const {
    if (!m_registry.valid(item)) return {};

    if (auto ptr = m_registry.try_get<IdentityComponent>(item); ptr) {
        return ptr->name;
    }

    return QString("Entity %1").arg(entt::to_integral(item.value));
}

entt::entity Database::add_material_group(QString               new_name,
                                          QVector<entt::entity> members,
                                          entt::entity          clone_from) {
    auto set = std::unordered_set(members.begin(), members.end());

    MaterialComponent params;

    if (m_registry.valid(clone_from) and
        m_registry.all_of<MaterialComponent>(clone_from)) {

        auto& other_p = m_registry.get<MaterialComponent>(clone_from);

        params = other_p;
    } else {
        params.optics_back.set_ideal_absorption();
        params.optics_front.set_ideal_reflection();
    }

    for (auto mem : members) {
        this->remove_material(mem);
    }

    auto ent = m_registry.create();
    m_registry.emplace<MaterialGroupComponent>(
        ent,
        MaterialGroupComponent {
            .members = QVector<entt::entity>(set.begin(), set.end()),
        });
    m_registry.emplace<MaterialComponent>(ent, params);

    m_registry.emplace<IdentityComponent>(
        ent, IdentityComponent { .name = new_name });

    for (auto child : set) {
        m_registry.emplace_or_replace<MaterialGroupMemberComponent>(
            child, MaterialGroupMemberComponent { .group = ent });
    }

    return ent;
}

void Database::delete_material_group(entt::entity to_delete,
                                     entt::entity move_to) {
    if (to_delete == move_to) {
        qWarning()
            << "Trying to delete a group and move members to the same group!";
        return;
    }

    // if not a group, bail
    if (!m_registry.all_of<MaterialGroupComponent>(to_delete)) { return; }

    // steal current member list
    auto members =
        std::move(m_registry.get<MaterialGroupComponent>(to_delete).members);

    // destroy current group entity
    m_registry.destroy(to_delete);

    if (m_registry.valid(move_to) and
        m_registry.all_of<MaterialGroupComponent>(move_to)) {
        // moving to valid target

        // reset member list membership
        for (auto child : members) {
            m_registry.emplace_or_replace<MaterialGroupMemberComponent>(
                child, MaterialGroupMemberComponent { .group = move_to });
        }

        m_registry.patch<MaterialGroupComponent>(
            move_to, [&](MaterialGroupComponent& a) {
                a.members.append(members.begin(), members.end());
            });
    } else {
        // invalid target. clear

        m_registry.remove<MaterialGroupMemberComponent>(members.begin(),
                                                        members.end());
    }
}

entt::entity Database::material_of(entt::entity element) const {
    if (auto* m = m_registry.try_get<MaterialGroupMemberComponent>(element)) {
        return m->group;
    }
    return entt::null;
}

entt::entity Database::add_geometry_group(QString               new_name,
                                          QVector<entt::entity> members,
                                          entt::entity          clone_from) {
    auto set = std::unordered_set(members.begin(), members.end());

    GeometryComponent params;

    if (m_registry.valid(clone_from) and
        m_registry.all_of<GeometryComponent>(clone_from)) {

        auto& other_p = m_registry.get<GeometryComponent>(clone_from);

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
        this->remove_geometry(mem);
    }

    auto ent = m_registry.create();
    m_registry.emplace<GeometryGroupComponent>(
        ent,
        GeometryGroupComponent {
            .members = QVector<entt::entity>(set.begin(), set.end()),
        });
    m_registry.emplace<GeometryComponent>(ent, params);

    m_registry.emplace<IdentityComponent>(
        ent, IdentityComponent { .name = new_name });

    for (auto child : set) {
        m_registry.emplace_or_replace<GeometryGroupMemberComponent>(
            child, GeometryGroupMemberComponent { .group = ent });
    }

    return ent;
}

void Database::delete_geometry_group(entt::entity to_delete,
                                     entt::entity move_to) {
    if (to_delete == move_to) {
        qWarning()
            << "Trying to delete a group and move members to the same group!";
        return;
    }

    // if not a group, bail
    if (!m_registry.all_of<GeometryGroupComponent>(to_delete)) { return; }

    // steal current member list
    auto members =
        std::move(m_registry.get<GeometryGroupComponent>(to_delete).members);

    // destroy current group entity
    m_registry.destroy(to_delete);

    if (m_registry.valid(move_to) and
        m_registry.all_of<GeometryGroupComponent>(move_to)) {
        // moving to valid target

        // reset member list membership
        for (auto child : members) {
            m_registry.emplace_or_replace<GeometryGroupMemberComponent>(
                child, GeometryGroupMemberComponent { .group = move_to });
        }

        m_registry.patch<GeometryGroupComponent>(
            move_to, [&](GeometryGroupComponent& a) {
                a.members.append(members.begin(), members.end());
            });
    } else {
        // invalid target. clear

        m_registry.remove<GeometryGroupMemberComponent>(members.begin(),
                                                        members.end());
    }
}

entt::entity Database::geometry_of(entt::entity element) const {
    if (auto* m = m_registry.try_get<GeometryGroupMemberComponent>(element)) {
        return m->group;
    }
    return entt::null;
}

void Database::select(entt::entity to_select) {
    clear_selection();
    selected.set(to_select, SelectedComponent {});
}

void Database::deselect(entt::entity to_deselect) {
    selected.remove(to_deselect);
}

void Database::toggle_selection(entt::entity to_toggle_selection) {
    if (m_registry.all_of<SelectedComponent>(to_toggle_selection)) {
        selected.remove(to_toggle_selection);
    } else {
        selected.set(to_toggle_selection, SelectedComponent {});
    }
}

void Database::clear_selection() {
    auto                      view = selected.view();
    std::vector<entt::entity> to_deselect(view.begin(), view.end());
    for (auto e : to_deselect)
        selected.remove(e);
}

bool Database::is_selected(entt::entity e) const {
    return m_registry.valid(e) && m_registry.all_of<SelectedComponent>(e);
}

void Database::set_color(entt::entity to_color, QColor new_color) {
    this->color.set(to_color, ColorComponent { .color = new_color });
}

} // namespace db
