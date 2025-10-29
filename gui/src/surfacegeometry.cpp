#include "surfacegeometry.h"

SurfaceGeometry::SurfaceGeometry(element_ptr elem) : m_element(elem), m_visible(true)
{
    assert(m_element != nullptr);
    rebuildGeometry();
}

void SurfaceGeometry::rebuildGeometry()
{
    SD::surface_ptr surface = m_element->get_surface();
    SD::aperture_ptr aperture = m_element->get_aperture();

    assert(surface != nullptr);
    assert(aperture != nullptr);

    auto indexed_tris = aperture->triangulation();

    std::vector<double> &points = std::get<0>(indexed_tris);
    std::vector<int> &indices = std::get<1>(indexed_tris);

    QList<Vertex> verts(points.size()/2);

    constexpr float maxFloat = std::numeric_limits<float>::max();
    QVector3D boundsMin(maxFloat, maxFloat, maxFloat);
    QVector3D boundsMax(-maxFloat, -maxFloat, -maxFloat);

    // Compute the positions
    for (int i = 0; i < verts.size(); ++i) {
        verts[i].position.setX(points[2*i]);
        verts[i].position.setY(points[2*i+1]);
        verts[i].position.setZ(surface->z(verts[i].position.x(), verts[i].position.y()));

        boundsMin.setX(std::min(boundsMin.x(), verts[i].position.x()));
        boundsMin.setY(std::min(boundsMin.y(), verts[i].position.y()));
        boundsMin.setZ(std::min(boundsMin.z(), verts[i].position.z()));

        boundsMax.setX(std::max(boundsMax.x(), verts[i].position.x()));
        boundsMax.setY(std::max(boundsMax.y(), verts[i].position.y()));
        boundsMax.setZ(std::max(boundsMax.z(), verts[i].position.z()));
    }

    // Compute the normals
    for (int i = 0; i < indices.size(); i+=3) {
        int a = indices[i];
        int b = indices[i+1];
        int c = indices[i+2];

        QVector3D normal = QVector3D::crossProduct(verts[a].position-verts[b].position,verts[b].position-verts[c].position);

        verts[a].normal += normal;
        verts[b].normal += normal;
        verts[c].normal += normal;
    }

    // Compute uv and normalize
    for (int i = 0; i < verts.size(); ++i) {
       \
        verts[i].normal.normalize();

        verts[i].uv.setX((verts[i].position.x()-boundsMin.x())/(boundsMax.x()-boundsMin.x()));
        verts[i].uv.setY((verts[i].position.y()-boundsMin.y())/(boundsMax.y()-boundsMin.y()));
    }

    QByteArray indexBuffer = QByteArray(reinterpret_cast<const char*>(indices.data()), indices.size() * sizeof(int));
    QByteArray vertexBuffer = QByteArray(reinterpret_cast<const char*>(verts.data()), verts.size() * sizeof(Vertex));

    addAttribute(QQuick3DGeometry::Attribute::PositionSemantic, 0,
                 QQuick3DGeometry::Attribute::ComponentType::F32Type);
    addAttribute(QQuick3DGeometry::Attribute::NormalSemantic, 3 * sizeof(float),
                 QQuick3DGeometry::Attribute::ComponentType::F32Type);
    addAttribute(QQuick3DGeometry::Attribute::IndexSemantic, 0,
                 QQuick3DGeometry::Attribute::ComponentType::U32Type);

    setStride(sizeof(Vertex));
    setVertexData(vertexBuffer);
    setIndexData(indexBuffer);
    setBounds(boundsMin, boundsMax);
    setPrimitiveType(QQuick3DGeometry::PrimitiveType::Triangles);
}

QVector3D SurfaceGeometry::position() const
{
    return QVector3D(m_element->get_origin_ref().data[0], m_element->get_origin_ref().data[1], m_element->get_origin_ref().data[2]);

}

QVector3D SurfaceGeometry::eulerAngles() const
{
    double dx = m_element->get_aim_vector_ref()[0]-m_element->get_origin_ref().data[0];
    double dy = m_element->get_aim_vector_ref()[1]-m_element->get_origin_ref().data[1];
    double dz = m_element->get_aim_vector_ref()[1]-m_element->get_origin_ref().data[2];

    double dt = sqrt(dx*dx + dy*dy + dz*dz);

    return QVector3D(dt != 0 ? asin(dy/dt) * 180.0/M_PI : 0,
                     dt != 0 ? atan2(dx/dt,dz/dt)  * 180.0/M_PI : 0,
                     m_element->get_zrot());
}

void SurfaceGeometry::setVisible(bool v)
{
    m_visible = v;
}

QString SurfaceGeometry::label() const
{
    return QString::fromStdString(m_element->get_surface()->get_type_string()) + " (" + QString::fromStdString(m_element->get_aperture()->get_type_string()) + ")";
}

bool SurfaceGeometry::operator==(const SurfaceGeometry& other) const
{
    return m_element == other.m_element;
}
