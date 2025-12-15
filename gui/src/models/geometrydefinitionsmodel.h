#pragma once

#include "../surfacegeometry.h"

#include "utilities/indirect_model.h"

#include <QUuid>
#include <QVector3D>


class GeometryDefinitionsModel
    : public ListContainerModel<std::shared_ptr<SurfaceGeometry>> {

    bool _can_append_new(QVariant const&) override { return false; }
    void _append_new(QVariant) override;

public:
    explicit GeometryDefinitionsModel(QObject* parent = nullptr);
};
