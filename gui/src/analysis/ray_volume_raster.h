#pragma once

#include "grid3d.h"

#include <QVector3D>

namespace analysis {


using Vec3i = std::array<int, 3>;

inline Vec3i floor(QVector3D x) {
    return { static_cast<int>(std::floor(x.x())),
             static_cast<int>(std::floor(x.y())),
             static_cast<int>(std::floor(x.z())) };
}

inline Vec3i ceil(QVector3D x) {
    return { static_cast<int>(std::ceil(x.x())),
             static_cast<int>(std::ceil(x.y())),
             static_cast<int>(std::ceil(x.z())) };
}

inline Vec3i
world_to_voxel(QVector3D const& p, QVector3D const& origin, float cellSize) {
    auto rel = (p - origin) / cellSize;
    return floor(rel);
}

template <typename T>
void raster_segment(Grid3D<T>&       grid,
                    QVector3D const& p0_grid,
                    QVector3D const& p1_grid) {
    // Direction in grid space
    QVector3D d = p1_grid - p0_grid;

    // Start and end voxels (integer indices)
    int x = static_cast<int>(std::floor(p0_grid.x()));
    int y = static_cast<int>(std::floor(p0_grid.y()));
    int z = static_cast<int>(std::floor(p0_grid.z()));

    int x1 = static_cast<int>(std::floor(p1_grid.x()));
    int y1 = static_cast<int>(std::floor(p1_grid.y()));
    int z1 = static_cast<int>(std::floor(p1_grid.z()));

    auto in_bounds = [&](int xi, int yi, int zi) {
        return xi >= 0 && yi >= 0 && zi >= 0 &&
               xi < static_cast<int>(grid.size_x()) &&
               yi < static_cast<int>(grid.size_y()) &&
               zi < static_cast<int>(grid.size_z());
    };

    // If both endpoints are outside and you don't want to handle clipping,
    // you can early out. More robust is to clip the segment to the grid box.
    if (!in_bounds(x, y, z) && !in_bounds(x1, y1, z1)) return;

    // Step direction per axis (-1, 0, or +1)
    int stepX = (d.x() > 0.f) ? 1 : (d.x() < 0.f ? -1 : 0);
    int stepY = (d.y() > 0.f) ? 1 : (d.y() < 0.f ? -1 : 0);
    int stepZ = (d.z() > 0.f) ? 1 : (d.z() < 0.f ? -1 : 0);

    float tMaxX, tMaxY, tMaxZ;
    float tDeltaX, tDeltaY, tDeltaZ;

    auto INF = std::numeric_limits<float>::infinity();

    // X axis
    if (stepX != 0) {
        float nextVoxelBoundary = (stepX > 0) ? (static_cast<float>(x) + 1.0f)
                                              : static_cast<float>(x);
        tMaxX                   = (nextVoxelBoundary - p0_grid.x()) / d.x();
        tDeltaX                 = 1.0f / std::fabs(d.x());
    } else {
        tMaxX   = INF;
        tDeltaX = INF;
    }

    // Y axis
    if (stepY != 0) {
        float nextVoxelBoundary = (stepY > 0) ? (static_cast<float>(y) + 1.0f)
                                              : static_cast<float>(y);
        tMaxY                   = (nextVoxelBoundary - p0_grid.y()) / d.y();
        tDeltaY                 = 1.0f / std::fabs(d.y());
    } else {
        tMaxY   = INF;
        tDeltaY = INF;
    }

    // Z axis
    if (stepZ != 0) {
        float nextVoxelBoundary = (stepZ > 0) ? (static_cast<float>(z) + 1.0f)
                                              : static_cast<float>(z);
        tMaxZ                   = (nextVoxelBoundary - p0_grid.z()) / d.z();
        tDeltaZ                 = 1.0f / std::fabs(d.z());
    } else {
        tMaxZ   = INF;
        tDeltaZ = INF;
    }

    // Make sure start voxel is inside before touching the grid
    if (in_bounds(x, y, z)) {
        grid(static_cast<size_t>(x),
             static_cast<size_t>(y),
             static_cast<size_t>(z)) += T(1);
    }

    // Traverse until we reach the voxel containing p1
    while (x != x1 || y != y1 || z != z1) {
        // Advance to next voxel boundary in the dimension with smallest tMax
        if (tMaxX < tMaxY) {
            if (tMaxX < tMaxZ) {
                x += stepX;
                tMaxX += tDeltaX;
            } else {
                z += stepZ;
                tMaxZ += tDeltaZ;
            }
        } else {
            if (tMaxY < tMaxZ) {
                y += stepY;
                tMaxY += tDeltaY;
            } else {
                z += stepZ;
                tMaxZ += tDeltaZ;
            }
        }

        if (!in_bounds(x, y, z)) break;

        grid(static_cast<size_t>(x),
             static_cast<size_t>(y),
             static_cast<size_t>(z)) += T(1);
    }
}


} // namespace analysis
