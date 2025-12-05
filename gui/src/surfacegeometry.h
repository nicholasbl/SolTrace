#pragma once

#include <QQuick3DGeometry>
#include <QVector3D>

#include "single_element.hpp"

namespace SD = SolTrace::Data;

class SurfaceGeometry : public QQuick3DGeometry {
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

    QVector3D aim() const;
    void setAim(QVector3D pt);

    float zRotation() const;
    void setZRotation(float theta);

    QString surfaceType() const;
    QString apertureType() const;

    bool operator==(const SurfaceGeometry& other) const;
};
