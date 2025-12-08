#pragma once

#include <QString>

#include "simulation_data_api.hpp"
#include "simulation_result.hpp"

namespace SD = SolTrace::Data;
namespace RD = SolTrace::Result;
using SimDataPtr = std::shared_ptr<SD::SimulationData>;

struct ResultDB {
    using Result = RD::SimulationResult;

    Result result;

    // look up by element id. Not clean right now
    std::unordered_map<SD::element_id, std::vector<RD::ray_id>>
        element_ids_to_ray_ids;
};

using ResultPtr = std::shared_ptr<ResultDB>;
using SimResult = std::variant<ResultPtr, QString>;
