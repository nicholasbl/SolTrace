#pragma once

#include "simulation_data_api.hpp"

#include "surfacegeometrylistmodel.h"
#include "indirect_model.h"

#include "job_run.h"

#include "qt_helpers.h"

#include <QQmlEngine>
#include <QVector3D>

namespace SD = SolTrace::Data;

using SimDataPtr = std::shared_ptr<SD::SimulationData>;

// NOTE that we are truncating vectors to floats...

inline QVector3D convert(SD::Vector3d v) {
    return QVector3D(v[0], v[1], v[2]);
}

inline SD::Vector3d convert(QVector3D v) {
    return { v.x(), v.y(), v.z() };
}

class RaySourceModel : public QObject {
    Q_OBJECT

    std::shared_ptr<SolTrace::Data::Sun> m_ray_source;

    using DistributionType = SolTrace::Data::DistributionType;

    Q_ENUM(DistributionType);

    Q_WRITABLE_PROPERTY(QVector3D, position, QVector3D());
    Q_WRITABLE_PROPERTY(DistributionType, shape, DistributionType::GAUSSIAN);
    Q_WRITABLE_PROPERTY(double, sigma, 0.0);
    Q_WRITABLE_PROPERTY(double, half_width, 0.0);
    Q_WRITABLE_PROPERTY(QVector<double>, user_angle, {});
    Q_WRITABLE_PROPERTY(QVector<double>, user_intensity, {});

public:
    RaySourceModel(SimDataPtr, QObject* parent = nullptr);

public slots:
    void commit();
};


class ElementTableModel : public HashContainerModel<SD::element_id> {
    Q_OBJECT

    SimDataPtr m_data;

    virtual bool _can_append_new(QVariant const&);
    virtual void _append_new(QVariant);
    virtual bool _can_delete_at(size_t, size_t);
    virtual void _delete_at(size_t, size_t);
    virtual void _clear();

    QOBJECT_WRITABLE_PROPERTY(SurfaceGeometryListModel, surface_geometries);
    QOBJECT_WRITABLE_PROPERTY(RayGeometry, ray_geometry);

    // Hack for now
    Q_WRITABLE_PROPERTY(bool, sim_running, false);

private slots:
    void sim_done();

public:
    explicit ElementTableModel(SimDataPtr, QObject* parent = nullptr);

    ~ElementTableModel();

public slots:
    void run_simulation();
};


class Data : public QObject {
    Q_OBJECT

    Q_WRITABLE_PROPERTY(bool, modified, false);

    QOBJECT_WRITABLE_PROPERTY(RaySourceModel, ray_source_model);
    QOBJECT_WRITABLE_PROPERTY(ElementTableModel, element_model);

private slots:
    void mark_changed();

public:
    Data(SimDataPtr, QObject* parent = nullptr);
};

using DataPtr = std::shared_ptr<Data>;
