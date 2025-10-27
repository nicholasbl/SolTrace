#include "surfacegeometry.h"

SurfaceGeometry::SurfaceGeometry(single_element_ptr se) : m_element(se) {
    rebuildGeometry();
}

void SurfaceGeometry::rebuildGeometry() {
    if (!m_element) return;

    auto surface  = m_element->get_surface();
    auto aperture = m_element->get_aperture();

    if (!surface) return;
    if (!aperture) return;

    auto [points, indices] = aperture->triangulation();

    QList<Vertex> verts(points.size() / 2);

    constexpr float maxFloat = std::numeric_limits<float>::max();

    QVector3D boundsMin(maxFloat, maxFloat, maxFloat);
    QVector3D boundsMax(-maxFloat, -maxFloat, -maxFloat);

    // Compute the positions
    for (int i = 0; i < verts.size(); ++i) {
        auto position =
            QVector3D(points[2 * i],
                      points[2 * i + 1],
                      surface->z(verts[i].position.x(), verts[i].position.y()));

        verts[i].position = position;


        boundsMin.setX(std::min(boundsMin.x(), position.x()));
        boundsMin.setY(std::min(boundsMin.y(), position.y()));
        boundsMin.setZ(std::min(boundsMin.z(), position.z()));

        boundsMax.setX(std::max(boundsMax.x(), position.x()));
        boundsMax.setY(std::max(boundsMax.y(), position.y()));
        boundsMax.setZ(std::max(boundsMax.z(), position.z()));
    }

    // Compute the normals
    for (int i = 0; i < indices.size(); i += 3) {
        int a = i;
        int b = i + 1;
        int c = i + 2;

        QVector3D normal =
            QVector3D::crossProduct(verts[a].position, verts[b].position);

        verts[a].normal += normal;
        verts[b].normal += normal;
        verts[c].normal += normal;
    }

    // Compute uv and normalize
    for (int i = 0; i < verts.size(); ++i) {
        verts[i].normal.normalize();

        verts[i].uv.setX((verts[i].position.x() - boundsMin.x()) /
                         (boundsMax.x() - boundsMin.x()));
        verts[i].uv.setY((verts[i].position.y() - boundsMin.y()) /
                         (boundsMax.y() - boundsMin.y()));
    }

    auto indexBuffer = QByteArray(reinterpret_cast<const char*>(indices.data()),
                                  indices.size() * sizeof(int));
    auto vertexBuffer = QByteArray(reinterpret_cast<const char*>(verts.data()),
                                   verts.size() * sizeof(Vertex));

    setStride(sizeof(Vertex));
    setVertexData(vertexBuffer);
    setIndexData(indexBuffer);
    setBounds(boundsMin, boundsMax);
    setPrimitiveType(QQuick3DGeometry::PrimitiveType::Triangles);
}
