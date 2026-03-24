#include "ray_volume.h"


namespace analysis {

void RayVolume::clean() {
    m_dirty = false;
    m_data.resize(m_grid.size() * sizeof(InstanceTableEntry));

    if (m_grid.size() == 0) { return; }

    auto delta = m_extents.x() / m_grid.size_x();

    auto dest = reinterpret_cast<InstanceTableEntry*>(m_data.data());

    for (int x = 0; x < m_grid.size_x(); x++) {
        for (int y = 0; y < m_grid.size_y(); y++) {
            for (int z = 0; z < m_grid.size_z(); z++) {
                auto p = QVector3D(x, y, z) * delta + m_origin;

                auto data = m_grid(x, y, z);

                // if (data > 0.5) { qDebug() << x << y << z << data << delta; }

                float scale = delta * data;

                dest[m_grid.index(x, y, z)] =
                    calculateTableEntry(p,
                                        QVector3D(scale, scale, scale),
                                        QVector3D(),
                                        QColor::fromRgb(255, 255, 255));
            }
        }
    }
}

RayVolume::RayVolume() {
    markDirty();
    m_dirty = true;
}

void RayVolume::set_grid(analysis::Grid3D<float>&& g,
                         QVector3D                 origin,
                         QVector3D                 extents) {
    m_grid    = std::move(g);
    m_origin  = origin;
    m_extents = extents;
    m_dirty   = true;
    markDirty();
}

QByteArray RayVolume::getInstanceBuffer(int* instance_count) {
    if (m_dirty) clean();

    if (instance_count) { *instance_count = m_grid.size(); }

    return m_data;
}

} // namespace analysis
