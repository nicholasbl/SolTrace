#pragma once

#include "analysis/grid2d.h"

#include <QImage>

namespace analysis {

struct BakedFluxMap {
    Grid2D<float> counts;
    QImage        image;
};

using BakedFluxMapPtr = std::shared_ptr<BakedFluxMap const>;

} // namespace analysis