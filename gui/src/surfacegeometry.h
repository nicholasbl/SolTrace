#ifndef SURFACEGEOMETRY_H
#define SURFACEGEOMETRY_H

#include <QQuick3DGeometry>

#include "single_element.hpp"

#include <QVector3D>

namespace SD = SolTrace::Data;

class SurfaceGeometry : public QQuick3DGeometry
{
    Q_OBJECT
    QML_NAMED_ELEMENT(SurfaceGeometry)

    std::shared_ptr<SD::Element> m_element;
    bool m_visible;

    void rebuildGeometry();

    struct Vertex {
        QVector3D position;
        QVector3D normal;
        QVector2D uv;
    };

public:
    using element_ptr = typename std::shared_ptr<SD::Element>;

    SurfaceGeometry(element_ptr);

    QVector3D position() const;
    QVector3D eulerAngles() const;

    bool visible() const { return m_visible; }
    void setVisible(bool visible);

    QString label() const;

    bool operator==(const SurfaceGeometry& other) const;
};

#endif // SURFACEGEOMETRY_H
