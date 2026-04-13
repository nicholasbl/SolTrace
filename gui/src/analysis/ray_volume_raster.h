#pragma once

#include "grid3d.h"

#include <glm/glm.hpp>
#include <glm/vec3.hpp>

namespace analysis {

inline glm::ivec3
world_to_voxel(glm::vec3 const& p, glm::vec3 const& origin, float cellSize) {
    auto rel = (p - origin) / cellSize;
    return glm::floor(rel);
}

template <typename T>
void raster_segment(Grid3D<T>&       grid,
                    glm::vec3 const& p0_grid,
                    glm::vec3 const& p1_grid) {
    // Direction in grid space
    glm::vec3 d = p1_grid - p0_grid;

    // Start and end voxels (integer indices)
    auto start_voxel = glm::ivec3(glm::floor(p0_grid));
    auto end_voxel   = glm::ivec3(glm::floor(p1_grid));

    auto in_bounds = [&](glm::ivec3 p) {
        return p.x >= 0 && p.y >= 0 && p.z >= 0 &&
               p.x < static_cast<int>(grid.size_x()) &&
               p.y < static_cast<int>(grid.size_y()) &&
               p.z < static_cast<int>(grid.size_z());
    };

    // If both endpoints are outside and you don't want to handle clipping,
    // you can early out. More robust is to clip the segment to the grid box.
    if (!in_bounds(start_voxel) && !in_bounds(end_voxel)) return;

    // Step direction per axis (-1, 0, or +1)
    int stepX = (d.x > 0.f) ? 1 : (d.x < 0.f ? -1 : 0);
    int stepY = (d.y > 0.f) ? 1 : (d.y < 0.f ? -1 : 0);
    int stepZ = (d.z > 0.f) ? 1 : (d.z < 0.f ? -1 : 0);

    float tMaxX, tMaxY, tMaxZ;
    float tDeltaX, tDeltaY, tDeltaZ;

    auto INF = std::numeric_limits<float>::infinity();

    // X axis
    if (stepX != 0) {
        float nextVoxelBoundary =
            (stepX > 0) ? (static_cast<float>(start_voxel.x) + 1.0f)
                        : static_cast<float>(start_voxel.x);
        tMaxX   = (nextVoxelBoundary - p0_grid.x) / d.x;
        tDeltaX = 1.0f / std::fabs(d.x);
    } else {
        tMaxX   = INF;
        tDeltaX = INF;
    }

    // Y axis
    if (stepY != 0) {
        float nextVoxelBoundary =
            (stepY > 0) ? (static_cast<float>(start_voxel.y) + 1.0f)
                        : static_cast<float>(start_voxel.y);

        tMaxY   = (nextVoxelBoundary - p0_grid.y) / d.y;
        tDeltaY = 1.0f / std::fabs(d.y);
    } else {
        tMaxY   = INF;
        tDeltaY = INF;
    }

    // Z axis
    if (stepZ != 0) {
        float nextVoxelBoundary =
            (stepZ > 0) ? (static_cast<float>(start_voxel.z) + 1.0f)
                        : static_cast<float>(start_voxel.z);
        tMaxZ   = (nextVoxelBoundary - p0_grid.z) / d.z;
        tDeltaZ = 1.0f / std::fabs(d.z);
    } else {
        tMaxZ   = INF;
        tDeltaZ = INF;
    }

    // Make sure start voxel is inside before touching the grid
    if (in_bounds(start_voxel)) {
        grid(static_cast<size_t>(start_voxel.x),
             static_cast<size_t>(start_voxel.y),
             static_cast<size_t>(start_voxel.z)) += T(1);
    }

    // Traverse until we reach the voxel containing p1
    while (start_voxel.x != end_voxel.x || start_voxel.y != end_voxel.y ||
           start_voxel.z != end_voxel.z) {
        // Advance to next voxel boundary in the dimension with smallest tMax
        if (tMaxX < tMaxY) {
            if (tMaxX < tMaxZ) {
                start_voxel.x += stepX;
                tMaxX += tDeltaX;
            } else {
                start_voxel.z += stepZ;
                tMaxZ += tDeltaZ;
            }
        } else {
            if (tMaxY < tMaxZ) {
                start_voxel.y += stepY;
                tMaxY += tDeltaY;
            } else {
                start_voxel.z += stepZ;
                tMaxZ += tDeltaZ;
            }
        }

        if (!in_bounds(start_voxel)) break;

        grid(static_cast<size_t>(start_voxel.x),
             static_cast<size_t>(start_voxel.y),
             static_cast<size_t>(start_voxel.z)) += T(1);
    }
}


} // namespace analysis
