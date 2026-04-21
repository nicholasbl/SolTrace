#pragma once

#include "utilities/grid2d.h"

#include <QImage>

namespace analysis {

/// A computed flux map
struct BakedFluxMap {
    /// A per-pixel normalized count, used with a color map to generate images
    Grid2D<float> counts;

    /// A generated image using counts and a color map
    QImage bin_map;

    /// A generated image showing UV points of intersecting rays
    QImage point_map;
};

using BakedFluxMapPtr = std::shared_ptr<BakedFluxMap const>;

} // namespace analysis
