#pragma once

#include <QObject>
#include <QStringListModel>
#include <QtGui/qvectornd.h>
#include <QtQuick3D/qquick3dgeometry.h>

#include "database/apertureeditor.h"
#include "database/components.h"
#include "database/database.h"
#include "database/surfaceeditor.h"
#include "utilities/qt_helpers.h"

#include "aperture.hpp"
#include "surface.hpp"

namespace SD = SolTrace::Data;


namespace db {

struct MaterialComponent;

class BoundingBox {
    Q_GADGET
    Q_PROPERTY(QVector3D min MEMBER min)
    Q_PROPERTY(QVector3D max MEMBER max)

public:
    QVector3D min;
    QVector3D max;

    bool operator==(BoundingBox const&) const = default;
};


/// Surface geometry visualization. Creates geometry for Quick3D for a given
/// group
class SurfaceGeometry : public QQuick3DGeometry, public DatabaseObserver {
    Q_OBJECT

    entt::entity m_current_group = entt::null;

    struct Vertex {
        QVector3D position;
        QVector3D normal;
        QVector2D uv;
    };

    void set_new_database_connections(Database* ptr) override;

private slots:
    void parameters_changed(entt::entity);
    void rebuild_geometry();

public:
    SurfaceGeometry();

    Q_READONLY_PROPERTY(BoundingBox, bounding_box)

    void set(Database*, entt::entity group);

public:
    void debug();
};


/// Model providing an interface to edit a group
class GeometryEditor : public QObject, public DatabaseObserver {
    Q_OBJECT

    entt::entity m_current_group = entt::null;

    void set_new_database_connections(Database* ptr) override;

    QOBJECT_WRITABLE_PROPERTY(SurfaceGeometry, surface_geometry);

    QString m_kind;
    Q_PROPERTY(QString aperture_kind READ kind WRITE set_kind NOTIFY
                   kind_changed FINAL)

    QString m_surf_kind;
    Q_PROPERTY(QString surface_kind READ surface_kind WRITE set_surface_kind
                   NOTIFY surface_kind_changed FINAL)

public:
    /// Coarse validation result for the currently selected surface/aperture.
    /// Exposed to QML so the UI can style warnings/errors immediately.
    enum class GeometryValidationStatus { Ok, Warning, Error };
    Q_ENUM(GeometryValidationStatus)

private:
    Q_PROPERTY(GeometryValidationStatus geometry_validation_status READ
                   geometry_validation_status NOTIFY
                       geometry_validation_status_changed FINAL)

    // UX Helpers
    QOBJECT_READONLY_PROPERTY(QStringListModel, surface_type_model);
    QOBJECT_READONLY_PROPERTY(QStringListModel, aperture_type_model);

    QOBJECT_READONLY_PROPERTY(ApertureParameterModel, aperture_parameter_model);
    QOBJECT_READONLY_PROPERTY(SurfaceParameterModel, surface_parameter_model);

    //
    void make_new_aperture(SD::ApertureType);
    void make_new_surface(SD::SurfaceType);

private slots:
    void parameters_changed(entt::entity);

    /// Recompute `geometry_validation_status` from current group parameters.
    void evaluate_geometry_validation();

public:
    explicit GeometryEditor(QObject* parent = nullptr);
    ~GeometryEditor() override;

    void set(Database*, entt::entity group);

public slots:
    QString kind() const;
    void    set_kind(QString newKind);

    QString surface_kind() const;
    void    set_surface_kind(QString newSurface_kind);

    /// Current geometry validation state for the selected group.
    GeometryValidationStatus geometry_validation_status() const;

signals:
    void updated();
    void kind_changed();
    void surface_kind_changed();
    void geometry_validation_status_changed();

private:
    GeometryValidationStatus m_geometry_validation_status =
        GeometryValidationStatus::Error;
};

} // namespace db

Q_DECLARE_METATYPE(db::BoundingBox)
