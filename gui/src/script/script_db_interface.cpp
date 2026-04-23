#include "script_db_interface.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>

#include <glm/gtc/quaternion.hpp>

#include <exception>
#include <utility>

namespace SolTrace::GUI::Script {

namespace {

QJsonArray to_json(glm::dvec3 const& v) {
    return QJsonArray { v.x, v.y, v.z };
}

QJsonArray to_json(glm::dquat const& q) {
    return QJsonArray { q.w, q.x, q.y, q.z };
}

bool read_vec3(QJsonValue const& value, glm::dvec3& out) {
    if (value.isArray()) {
        auto array = value.toArray();
        if (array.size() != 3) return false;
        out = glm::dvec3 {
            array.at(0).toDouble(),
            array.at(1).toDouble(),
            array.at(2).toDouble(),
        };
        return true;
    }

    if (value.isObject()) {
        auto obj = value.toObject();
        out      = glm::dvec3 {
            obj.value("x").toDouble(),
            obj.value("y").toDouble(),
            obj.value("z").toDouble(),
        };
        return obj.contains("x") && obj.contains("y") && obj.contains("z");
    }

    return false;
}

bool read_quat(QJsonValue const& value, glm::dquat& out) {
    if (value.isArray()) {
        auto array = value.toArray();
        if (array.size() != 4) return false;
        out = glm::dquat {
            array.at(0).toDouble(),
            array.at(1).toDouble(),
            array.at(2).toDouble(),
            array.at(3).toDouble(),
        };
        return true;
    }

    if (value.isObject()) {
        auto obj = value.toObject();
        out      = glm::dquat {
            obj.value("w").toDouble(),
            obj.value("x").toDouble(),
            obj.value("y").toDouble(),
            obj.value("z").toDouble(),
        };
        return obj.contains("w") && obj.contains("x") && obj.contains("y") &&
               obj.contains("z");
    }

    return false;
}

QJsonObject to_qjson(nlohmann::ordered_json const& json) {
    auto bytes = QByteArray::fromStdString(json.dump());
    return QJsonDocument::fromJson(bytes).object();
}

nlohmann::ordered_json to_njson(QJsonObject const& object) {
    auto bytes = QJsonDocument(object).toJson(QJsonDocument::Compact);
    return nlohmann::ordered_json::parse(bytes.constData());
}

QJsonObject to_qjson(SD::OpticalProperties const& properties) {
    nlohmann::ordered_json json;
    properties.write_json(json);
    return to_qjson(json);
}

void patch_optical_properties(SD::OpticalProperties& properties,
                              QJsonObject const&     object) {
    if (object.contains("my_type")) {
        auto key = object.value("my_type").toString().toStdString();
        if (auto value = db::reverse_lookup(SD::InteractionTypeMap, key)) {
            properties.my_type = *value;
        }
    }

    if (object.contains("error_distribution_type")) {
        auto key =
            object.value("error_distribution_type").toString().toStdString();
        if (auto value = db::reverse_lookup(SD::DistributionTypeMap, key)) {
            properties.error_distribution_type = *value;
        }
    }

    if (object.contains("transmissivity")) {
        properties.transmitivity = object.value("transmissivity").toDouble();
    }
    if (object.contains("transmitivity")) {
        properties.transmitivity = object.value("transmitivity").toDouble();
    }
    if (object.contains("reflectivity")) {
        properties.reflectivity = object.value("reflectivity").toDouble();
    }
    if (object.contains("slope_error")) {
        properties.slope_error = object.value("slope_error").toDouble();
    }
    if (object.contains("specularity_error")) {
        properties.specularity_error =
            object.value("specularity_error").toDouble();
    }
    if (object.contains("refraction_index_front")) {
        properties.refraction_index_front =
            object.value("refraction_index_front").toDouble();
    }
    if (object.contains("refraction_index_back")) {
        properties.refraction_index_back =
            object.value("refraction_index_back").toDouble();
    }
}

template <class Component>
QVector<db::Entity> collect_entities(entt::registry const& registry) {
    QVector<db::Entity> ret;
    for (auto entity : registry.view<Component const>()) {
        ret.push_back(db::Entity { entity });
    }
    return ret;
}

} // namespace

ScriptDBInterface::ScriptDBInterface(db::Database* database, QObject* parent)
    : QObject { parent }, m_database { database } { }

db::Entity ScriptDBInterface::create() {
    if (!m_database) return {};

    auto  entity = m_database->create();
    auto& reg    = m_database->as_registry();

    reg.emplace<db::ElementComponent>(entity);
    reg.emplace<db::TransformComponent>(
        entity,
        db::TransformComponent {
            .position = glm::dvec3 { 0.0 },
            .rotation = glm::dquat { 1.0, 0.0, 0.0, 0.0 },
        });

    return db::Entity { entity };
}

void ScriptDBInterface::destroy(db::Entity entity) {
    if (!m_database || !m_database->valid(entity)) return;

    auto& reg = m_database->as_registry();

    if (reg.all_of<db::MaterialGroupComponent>(entity)) {
        m_database->delete_material_group(entity);
        return;
    }

    if (reg.all_of<db::GeometryGroupComponent>(entity)) {
        m_database->delete_geometry_group(entity);
        return;
    }

    if (reg.all_of<db::TagComponent>(entity)) {
        m_database->delete_tag(entity);
        return;
    }

    if (auto children = m_database->children_of(entity); !children.empty()) {
        QVector<entt::entity> copy;
        copy.reserve(children.size());
        for (auto child : children) {
            copy.push_back(child);
        }
        for (auto child : std::as_const(copy)) {
            m_database->unset_parent(child);
        }
    }

    m_database->unset_parent(entity);
    m_database->remove_material(entity);
    m_database->remove_geometry(entity);

    auto                  tags = m_database->tags_for(entity);
    QVector<entt::entity> tag_copy;
    tag_copy.reserve(tags.size());
    for (auto tag : tags) {
        tag_copy.push_back(tag);
    }
    for (auto tag : std::as_const(tag_copy)) {
        m_database->unassign_tag(entity, tag);
    }

    reg.destroy(entity);
}

bool ScriptDBInterface::valid(db::Entity entity) {
    return m_database && m_database->valid(entity);
}

QString ScriptDBInterface::get_identity(db::Entity entity) {
    if (!m_database || !m_database->valid(entity)) return {};
    return m_database->name_of(entity);
}

void ScriptDBInterface::set_identity(db::Entity entity, QString name) {
    if (!m_database || !m_database->valid(entity)) return;
    m_database->identity.set(entity, db::IdentityComponent { .name = name });
}

bool ScriptDBInterface::get_invisible(db::Entity entity) {
    if (!m_database || !m_database->valid(entity)) return false;
    return m_database->as_registry().all_of<db::InvisibleComponent>(entity);
}

void ScriptDBInterface::set_invisible(db::Entity entity, bool invisible) {
    if (!m_database || !m_database->valid(entity)) return;

    if (invisible) {
        m_database->invisible.set(entity, db::InvisibleComponent {});
    } else if (get_invisible(entity)) {
        m_database->invisible.remove(entity);
    }
}

QJsonObject ScriptDBInterface::get_transform(db::Entity entity) {
    if (!m_database) return {};

    auto* transform = m_database->transform.get(entity);
    if (!transform) return {};

    return QJsonObject {
        { "position", to_json(transform->position) },
        { "rotation", to_json(transform->rotation) },
    };
}

void ScriptDBInterface::set_transform(db::Entity entity, QJsonObject object) {
    if (!m_database || !m_database->valid(entity)) return;

    m_database->transform.patch(entity, [&](db::TransformComponent& transform) {
        glm::dvec3 position;
        if (object.contains("position") &&
            read_vec3(object.value("position"), position)) {
            transform.position = position;
        }

        glm::dquat rotation;
        if (object.contains("rotation") &&
            read_quat(object.value("rotation"), rotation)) {
            transform.rotation = glm::normalize(rotation);
        }
    });
}

QVector<db::Entity> ScriptDBInterface::get_all_materials() {
    if (!m_database) return {};
    return collect_entities<db::MaterialGroupComponent>(
        m_database->as_registry());
}

db::Entity ScriptDBInterface::create_material() {
    if (!m_database) return {};
    return db::Entity { m_database->add_material_group("Material", {}) };
}

QJsonObject ScriptDBInterface::get_material_properties(db::Entity entity) {
    if (!m_database) return {};

    auto* material = m_database->material_parameters.get(entity);
    if (!material) return {};

    return QJsonObject {
        { "front", to_qjson(material->optics_front) },
        { "back", to_qjson(material->optics_back) },
    };
}

void ScriptDBInterface::set_material_properties(db::Entity  entity,
                                                QJsonObject object) {
    if (!m_database || !m_database->valid(entity)) return;

    m_database->material_parameters.try_patch(
        entity, [&](db::MaterialComponent& material) {
            if (object.value("front").isObject()) {
                patch_optical_properties(material.optics_front,
                                         object.value("front").toObject());
            }

            if (object.value("back").isObject()) {
                patch_optical_properties(material.optics_back,
                                         object.value("back").toObject());
            }
        });
}

void ScriptDBInterface::remove_material(db::Entity entity) {
    if (!m_database) return;
    m_database->delete_material_group(entity);
}

QVector<db::Entity> ScriptDBInterface::get_all_geometries() {
    if (!m_database) return {};
    return collect_entities<db::GeometryGroupComponent>(
        m_database->as_registry());
}

db::Entity ScriptDBInterface::create_geometry() {
    if (!m_database) return {};
    return db::Entity { m_database->add_geometry_group("Geometry", {}) };
}

QJsonObject ScriptDBInterface::get_geometry_properties(db::Entity entity) {
    if (!m_database) return {};

    auto* geometry = m_database->geometry_parameters.get(entity);
    if (!geometry) return {};

    QJsonObject ret;

    if (geometry->aperture) {
        nlohmann::ordered_json json;
        geometry->aperture->write_json(json);
        ret["aperture"] = to_qjson(json);
    }

    if (geometry->surface) {
        nlohmann::ordered_json json;
        geometry->surface->write_json(json);
        ret["surface"] = to_qjson(json);
    }

    return ret;
}

void ScriptDBInterface::set_geometry_properties(db::Entity  entity,
                                                QJsonObject object) {
    if (!m_database || !m_database->valid(entity)) return;

    m_database->geometry_parameters.try_patch(
        entity, [&](db::GeometryComponent& geometry) {
            try {
                if (object.value("aperture").isObject()) {
                    geometry.aperture = SD::Aperture::make_aperture_from_json(
                        to_njson(object.value("aperture").toObject()));
                }

                if (object.value("surface").isObject()) {
                    geometry.surface = SD::make_surface_from_json(
                        to_njson(object.value("surface").toObject()));
                }
            } catch (std::exception const& e) {
                qWarning() << "Failed to set geometry properties:" << e.what();
            }
        });
}

void ScriptDBInterface::remove_geometry(db::Entity entity) {
    if (!m_database) return;
    m_database->delete_geometry_group(entity);
}

db::Entity ScriptDBInterface::get_material_of(db::Entity entity) {
    if (!m_database || !m_database->valid(entity)) return {};
    return db::Entity { m_database->material_of(entity) };
}

void ScriptDBInterface::set_material_of(db::Entity entity,
                                        db::Entity material) {
    if (!m_database || !m_database->valid(entity)) return;
    m_database->assign_material(entity, material);
}

db::Entity ScriptDBInterface::get_geometry_of(db::Entity entity) {
    if (!m_database || !m_database->valid(entity)) return {};
    return db::Entity { m_database->geometry_of(entity) };
}

void ScriptDBInterface::set_geometry_of(db::Entity entity,
                                        db::Entity geometry) {
    if (!m_database || !m_database->valid(entity)) return;
    m_database->assign_geometry(entity, geometry);
}

} // namespace SolTrace::GUI::Script
