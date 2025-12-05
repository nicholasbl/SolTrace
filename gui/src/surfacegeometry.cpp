#include "surfacegeometry.h"

SurfaceGeometry::SurfaceGeometry(element_ptr elem)
    : m_element(elem), m_visible(true) {
    assert(m_element != nullptr);
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
        auto x = points[2 * i];
        auto y = points[2 * i + 1];
        auto position =
            QVector3D(x,
                      y,
                      surface->z(x,y));

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
        int a = indices[i];
        int b = indices[i + 1];
        int c = indices[i + 2];

        QVector3D normal =
            QVector3D::crossProduct(verts[a].position - verts[b].position,
                                    verts[b].position - verts[c].position);

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

    addAttribute(QQuick3DGeometry::Attribute::PositionSemantic,
                 0,
                 QQuick3DGeometry::Attribute::ComponentType::F32Type);
    addAttribute(QQuick3DGeometry::Attribute::NormalSemantic,
                 3 * sizeof(float),
                 QQuick3DGeometry::Attribute::ComponentType::F32Type);
    addAttribute(QQuick3DGeometry::Attribute::IndexSemantic,
                 0,
                 QQuick3DGeometry::Attribute::ComponentType::U32Type);

    setStride(sizeof(Vertex));
    setVertexData(vertexBuffer);
    setIndexData(indexBuffer);
    setBounds(boundsMin, boundsMax);
    setPrimitiveType(QQuick3DGeometry::PrimitiveType::Triangles);
}

QVector3D SurfaceGeometry::position() const {
    return QVector3D(m_element->get_origin_ref().data[0],
                     m_element->get_origin_ref().data[1],
                     m_element->get_origin_ref().data[2]);
}

QVector3D SurfaceGeometry::eulerAngles() const {
    double dx = m_element->get_aim_vector_ref()[0] -
                m_element->get_origin_ref().data[0];
    double dy = m_element->get_aim_vector_ref()[1] -
                m_element->get_origin_ref().data[1];
    double dz = m_element->get_aim_vector_ref()[1] -
                m_element->get_origin_ref().data[2];

    double dt = sqrt(dx * dx + dy * dy + dz * dz);

    return QVector3D(dt != 0 ? asin(dy / dt) * 180.0 / M_PI : 0,
                     dt != 0 ? atan2(dx / dt, dz / dt) * 180.0 / M_PI : 0,
                     m_element->get_zrot());
}

void SurfaceGeometry::setVisible(bool v) {
    m_visible = v;
}


QString SurfaceGeometry::surfaceType() const
{
    if (m_element->get_surface() == nullptr)
    {
        if (m_element->is_stage())
            return "Stage (" + QString::number(m_element->get_number_of_elements()) + ")";

        if (m_element->is_composite())
            return "Composite (" + QString::number(m_element->get_number_of_elements()) + ")";

        return "Bad Element";
    }

    return QString::fromStdString(m_element->get_surface()->get_type_string());
}

QString SurfaceGeometry::apertureType() const
{
    if(m_element->get_aperture() != nullptr)
    {
        QString aperture = QString::fromStdString(m_element->get_aperture()->get_type_string());

        return aperture;
    }
    return "";
}

QVector3D SurfaceGeometry::aim() const
{
    SD::Vector3d pt = m_element->get_aim_vector_ref();
    return QVector3D(pt.data[0], pt.data[0], pt.data[0]);
}

void SurfaceGeometry::setAim(QVector3D pt)
{
    m_element->set_aim_vector(pt.x(), pt.y(), pt.z());
    m_element->compute_coordinate_rotations();
    qDebug() << eulerAngles();
}

float SurfaceGeometry::zRotation() const
{
    return m_element->get_zrot();
}

void SurfaceGeometry::setZRotation(float theta)
{
    m_element->set_zrot(theta);
    m_element->compute_coordinate_rotations();
    qDebug() << eulerAngles();
}

bool SurfaceGeometry::operator==(const SurfaceGeometry& other) const {
    return m_element == other.m_element;
}
