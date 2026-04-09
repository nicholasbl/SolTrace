#pragma once

#include "analysis/grid3d.h"

#include <simulation_data.hpp>
#include <simulation_result.hpp>

#include <entt/entity/fwd.hpp>
#include <glm/ext/vector_double3.hpp>

#include <vector>

namespace db {

struct SimulationResultConversion {
    SolTrace::Result::SimulationResult&                                 result;
    SolTrace::Data::SimulationData const&                               data;
    std::unordered_map<SolTrace::Data::element_id, entt::entity> const& map;
};

// can probably make this a variant.
// create and exit dont need entities

enum class RayEventType : uint8_t {
    CREATE   = 1,
    ABSORB   = 2,
    REFLECT  = 3,
    TRANSMIT = 4,
    VIRTUAL  = 5,
    EXIT     = 6,
    UNKNOWN  = UINT8_MAX
};

struct RayEvent {
    glm::dvec3   location;
    glm::dvec3   direction;
    entt::entity entity;
    RayEventType event;
};

struct RayRecord {
    uint64_t              id;
    std::vector<RayEvent> events;
};

struct SimulationResult {
    std::vector<RayRecord> records;

    glm::dvec3 bounds_min;
    glm::dvec3 bounds_max;

    analysis::Grid3D<float> ray_volume;

    std::unordered_map<entt::entity, std::vector<uint64_t>> entity_to_ray_ids;

    // TODO: Why is this fallible?
    static std::shared_ptr<SimulationResult>
    convert(SimulationResultConversion const&);
};

} // namespace db
