#pragma once

#include <entt/entt.hpp>

#include "database/components.h"


// TODO BETTER COMPOSITE SUPPORT

namespace db {

/// Create a new simulation database
static std::shared_ptr<entt::registry> create_new();

/// Convert a Soltrace dataset to a database
static std::shared_ptr<entt::registry> import(SD::SimulationData&);

/// Convert a database back into a Soltrace dataset
static std::shared_ptr<SolTrace::Data::SimulationData>
export_to_simdata(entt::registry&);

/// Clear the active parent of an entity
void unset_parent(entt::registry&, entt::entity child);

/// Set the parent of an entity
void set_parent(entt::registry&, entt::entity child, entt::entity parent);

/// Get the list of children of this entity. Returns an empty list if there are
/// none.
std::span<entt::entity const> children_of(entt::registry&, entt::entity parent);

/// Assign an entity to a geometry group
void assign_group(entt::registry&, entt::entity child, entt::entity group);

/// Create a new tag. Note that tag names should be unique.
entt::entity create_tag(entt::registry&, QString name);

/// Ask if an entity has been given a tag
bool is_tagged(entt::registry&, entt::entity item, entt::entity tag);

/// Assign an entity to a specific tag
void assign_tag(entt::registry&, entt::entity item, entt::entity tag);

/// Remove a tag from an entity
void unassign_tag(entt::registry&, entt::entity item, entt::entity tag);

/// Clear and destroy a specific tag
void delete_tag(entt::registry&, entt::entity tag);

/// Get all the tags for an entity
std::span<entt::entity const> tags_for(entt::registry&, entt::entity item);

/// Get the name of an entity, either using the Identity component, or by using
/// the entity ID.
QString name_of(entt::registry&, entt::entity item);

/// Get the global ray source of the database
SD::ray_source_ptr get_ray_source(entt::registry const&);

/// Get the global simulation parameters
SD::SimulationParameters& get_sim_params(entt::registry&);

} // namespace db
