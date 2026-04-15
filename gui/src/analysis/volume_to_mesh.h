#pragma once

#include "analysis/grid3d.h"
#include "database/mesh.h"

#include <QPromise>
#include <glm/vec3.hpp>

namespace analysis {

void volume_to_mesh(QPromise<db::Mesh>& output,
                    Grid3D<float>       volume,
                    glm::vec3 const&    bounds_min,
                    glm::vec3 const&    bounds_max,
                    float               isoval);

}
