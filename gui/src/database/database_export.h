#pragma once

#include <entt/entity/entity.hpp>

#include "simulation_data.hpp"

#include <memory>
#include <unordered_map>

namespace db {

class Database;

/// A packed export of a database to the SolTrace data format.
struct DatabaseExport {
    std::shared_ptr<SolTrace::Data::SimulationData>              data;
    std::unordered_map<SolTrace::Data::element_id, entt::entity> element_map;
    std::unique_ptr<Database> source_database;

    DatabaseExport() = default;

    DatabaseExport(DatabaseExport const&)            = delete;
    DatabaseExport& operator=(DatabaseExport const&) = delete;
    DatabaseExport(DatabaseExport&&)                 = default;
    DatabaseExport& operator=(DatabaseExport&&)      = default;
};

} // namespace db
