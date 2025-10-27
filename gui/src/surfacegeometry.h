#pragma once

#include <QQuick3DGeometry>
#include <QVector3D>

#include "single_element.hpp"

namespace SD = SolTrace::Data;

class SurfaceGeometry : public QQuick3DGeometry {
    Q_OBJECT
    QML_NAMED_ELEMENT(SurfaceGeometry)

    std::shared_ptr<SD::SingleElement> m_element;

    void rebuildGeometry();

    struct Vertex {
        QVector3D position;
        QVector3D normal;
        QVector2D uv;
    };

public:
    using single_element_ptr = typename std::shared_ptr<SD::SingleElement>;

    SurfaceGeometry(single_element_ptr);
};
