#pragma once

#include <QObject>
#include <QVector>

#include <entt/entt.hpp>

#include <glm/gtc/quaternion.hpp>

#include "simulation_data_api.hpp"

namespace SD = SolTrace::Data;

namespace SolTrace {
namespace Data {
bool operator==(SD::OpticalProperties const& a, SD::OpticalProperties const& b);
}
} // namespace SolTrace


namespace db {

/// Tag component for visually hiding an element
struct InvisibleComponent { };

/// Tag for disabling an element in the simulation
struct DisabledComponent { };

/// An identity, or non-unique name for an entity (various semantics)
struct IdentityComponent {
    QString name;
};

struct ElementComponent { };

/// Describes the parent of this entity, if it has one. DO NOT modify this
/// component directly!
struct ChildOfComponent {
    entt::entity parent;
};

/// Describes the children of an entity. DO NOT modify this component directly!
struct ChildrenComponent {
    QVector<entt::entity> children;
};

/// Describe the attitude of this entity.
struct TransformComponent {
    glm::dvec3 position;
    glm::dquat rotation;

    glm::dmat4 as_matrix() const;
};

/// A Global describing the ray source.
struct RaySourceResource {
    SD::ray_source_ptr source;
};

/// A group's collective properties.
struct GroupParameterComponent {
    SD::aperture_ptr aperture;
    SD::surface_ptr  surface;

    SD::OpticalProperties optics_front;
    SD::OpticalProperties optics_back;

    bool operator==(GroupParameterComponent const&) const;
};

/// A group of common 'elements'. DO NOT modify the member information directly.
/// Other aspects can be modified at will.
struct GroupComponent {
    QVector<entt::entity> members;
};

/// Describes the group this entity belongs to. UDO NOT modify this component
/// directly!
struct GroupMemberComponent {
    entt::entity group;
};

/// A component indicating this entity is a Tag description
struct TagComponent { };

/// This is a tag component that is used, with a string, to indicate membership
/// in a tag. Do not modify this directly!
struct ATagMemberComponent { };

/// Lists the string tags this entity has
struct TagMembershipComponent {
    QVector<entt::entity> tags;
};

/// Denotes an entity that could not be imported properly
struct ImportErrorComponent {
    QString reason;
};

} // namespace db
