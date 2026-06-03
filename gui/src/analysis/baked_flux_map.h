#pragma once

#include "utilities/grid2d.h"

#include <QImage>
#include <QtGlobal>
#include <QVector3D>

namespace analysis {

/// Summary statistics computed with the baked flux map.
struct BakedFluxMapStats {
    /// Total rays in the simulation result used for this map.
    quint64 source_ray_count = 0;

    /// Number of plotted ray/surface interactions that contributed to this map.
    quint64 plotted_ray_count = 0;

    /// Current normalized energy per plotted ray interaction.
    double power_per_ray = 1.0;

    /// Integrated normalized power represented by plotted interactions.
    double plotted_power = 0.0;

    /// Flux statistics over the rasterized map values.
    double peak_flux    = 0.0;
    double min_flux     = 0.0;
    double average_flux = 0.0;
    double sigma_flux   = 0.0;
    double uniformity   = 0.0;

    /// Monte Carlo uncertainty estimates in percent where counts are known.
    double peak_flux_uncertainty    = 0.0;
    double average_flux_uncertainty = 0.0;

    /// Average world-space location of plotted interactions.
    QVector3D centroid;
};

/// A computed flux map
struct BakedFluxMap {
    /// A per-pixel normalized count, used with a color map to generate images
    Grid2D<float> counts;

    /// A generated image using counts and a color map
    QImage bin_map;

    /// A generated image showing UV points of intersecting rays
    QImage point_map;

    /// Summary statistics for the generated map.
    BakedFluxMapStats stats;
};

using BakedFluxMapPtr = std::shared_ptr<BakedFluxMap const>;

} // namespace analysis
