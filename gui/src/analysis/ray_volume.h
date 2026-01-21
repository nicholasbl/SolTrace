#pragma once

#include "analysis/grid3d.h"

#include <QtQuick3D/qquick3dinstancing.h>

namespace analysis {

class RayVolume : public QQuick3DInstancing {
    Q_OBJECT

    analysis::Grid3D<float> m_grid;
    QVector3D               m_origin;
    QVector3D               m_extents;
    bool                    m_dirty = true;
    QByteArray              m_data;

    void clean();

public:
    explicit RayVolume();

    void
    set_grid(analysis::Grid3D<float>&&, QVector3D origin, QVector3D extents);

    QByteArray getInstanceBuffer(int* instance_count) override;
};

} // namespace analysis
