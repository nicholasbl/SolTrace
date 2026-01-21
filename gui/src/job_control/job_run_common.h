#pragma once

#include <QString>
#include <QtCore/qpromise.h>
#include <QtGui/qvectornd.h>

#include "analysis/grid3d.h"
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

    QVector3D bounds_min;
    QVector3D bounds_max;

    analysis::Grid3D<float> ray_volume_grid;
};

using ResultPtr = std::shared_ptr<ResultDB>;
using SimResult = std::variant<ResultPtr, QString>;

/// Add results from a simulation. We don't use a constructor here as
/// it does not play well with the progress update and cancel concept
void construct_result(QPromise<SimResult>& promise,
                      ResultPtr            ptr,
                      SimDataPtr           data);
