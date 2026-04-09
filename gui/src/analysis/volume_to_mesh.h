#pragma once

#include "analysis/grid3d.h"
#include "database/mesh.h"

#include <QPromise>

namespace analysis {

void volume_to_mesh(QPromise<db::Mesh>& output,
                    Grid3D<float>       volume,
                    float               isoval);

}
