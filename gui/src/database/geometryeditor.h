#pragma once

#include <QObject>
#include <QtGui/qvectornd.h>
#include <QtQuick3D/qquick3dgeometry.h>

#include "database/components.h"
#include "database/database.h"
#include "qt_helpers.h"

#include "aperture.hpp"
#include "surface.hpp"

namespace SD = SolTrace::Data;


namespace db {

struct GroupParameterComponent;

/// Surface geometry visualization. Creates geometry for Quick3D for a given
/// group
class SurfaceGeometry : public QQuick3DGeometry, public DatabaseObserver {
    Q_OBJECT

    entt::entity m_current_group;

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

    void set(Database*, entt::entity group);
};


/// Model providing an interface to edit a group
class GroupEditor : public QObject, public DatabaseObserver {
    Q_OBJECT

    // TODO: Front and back optics

public:
    enum class ApertureKind {
        Annulus,
        Circle,
        Hexagon,
        Rectangle,
        Equilateral_Triangle,
        Single_Axis_Curvature_Section,
        Irregular_Triangle,
        Irregular_Quadrilateral,
        Unknown
    };
    Q_ENUM(ApertureKind);

    enum class SurfaceKind { Cone, Cylinder, Flat, Parabola, Sphere, Unknown };
    Q_ENUM(SurfaceKind);

private:
    entt::entity m_current_group;

    void set_new_database_connections(Database* ptr) override;

    QOBJECT_WRITABLE_PROPERTY(SurfaceGeometry, surface_geometry);

    /// Content for the aperture is defined by the Kind. See wrappers below.
    QOBJECT_WRITABLE_PROPERTY(QObject, aperture_editor);

    ApertureKind m_kind;
    Q_PROPERTY(ApertureKind aperture_kind READ kind WRITE set_kind NOTIFY
                   kind_changed FINAL)

    SurfaceKind m_surf_kind;
    Q_PROPERTY(SurfaceKind surface_kind READ surface_kind WRITE set_surface_kind
                   NOTIFY surface_kind_changed FINAL)

    // the surface information is usually packed into lists

    Q_WRITABLE_PROPERTY(QVector<double>, surface_arguments, {});

    void make_new_aperture(ApertureKind);
    void make_new_surface(SurfaceKind);

private slots:
    void parameters_changed(entt::entity);

public:
    explicit GroupEditor(QObject* parent = nullptr);
    ~GroupEditor() override;

    void set(Database*, entt::entity group);

public slots:
    ApertureKind kind() const;
    void         set_kind(ApertureKind newKind);

    SurfaceKind surface_kind() const;
    void        set_surface_kind(SurfaceKind newSurface_kind);

signals:
    void updated();
    void kind_changed();
    void surface_kind_changed();
};


// Wrappers for specific Apertures.

// -------------------- Annulus --------------------

class AnnulusWrapper : public QObject {
    Q_OBJECT

    SD::Annulus* m_ptr = nullptr;

    Q_WRITABLE_PROPERTY_CUSTOM_STORAGE(double,
                                       inner_radius,
                                       m_ptr->inner_radius);
    Q_WRITABLE_PROPERTY_CUSTOM_STORAGE(double,
                                       outer_radius,
                                       m_ptr->outer_radius);
    Q_WRITABLE_PROPERTY_CUSTOM_STORAGE(double, arc_angle, m_ptr->arc_angle);

public:
    explicit AnnulusWrapper(SD::Annulus* ptr, QObject* parent = nullptr)
        : QObject(parent), m_ptr(ptr) {
        // Forward all property signals to a single "changed"
        connect(this,
                &AnnulusWrapper::inner_radius_changed,
                this,
                &AnnulusWrapper::changed);
        connect(this,
                &AnnulusWrapper::outer_radius_changed,
                this,
                &AnnulusWrapper::changed);
        connect(this,
                &AnnulusWrapper::arc_angle_changed,
                this,
                &AnnulusWrapper::changed);
    }

    ~AnnulusWrapper() override = default;

    SD::Annulus* raw() const { return m_ptr; }

signals:
    void changed();
};

// -------------------- Circle --------------------

class CircleWrapper : public QObject {
    Q_OBJECT

    SD::Circle* m_ptr = nullptr;

    Q_WRITABLE_PROPERTY_CUSTOM_STORAGE(double, diameter, m_ptr->diameter);

public:
    explicit CircleWrapper(SD::Circle* ptr, QObject* parent = nullptr)
        : QObject(parent), m_ptr(ptr) {
        connect(this,
                &CircleWrapper::diameter_changed,
                this,
                &CircleWrapper::changed);
    }

    ~CircleWrapper() override = default;

    SD::Circle* raw() const { return m_ptr; }

signals:
    void changed();
};

// -------------------- EquilateralTriangle --------------------

// Spelling to match library
class EqualateralTriangleWrapper : public QObject {
    Q_OBJECT

    SD::EqualateralTriangle* m_ptr = nullptr;

    Q_WRITABLE_PROPERTY_CUSTOM_STORAGE(double,
                                       circumscribe_diameter,
                                       m_ptr->circumscribe_diameter);

public:
    explicit EqualateralTriangleWrapper(SD::EqualateralTriangle* ptr,
                                        QObject* parent = nullptr)
        : QObject(parent), m_ptr(ptr) {
        connect(this,
                &EqualateralTriangleWrapper::circumscribe_diameter_changed,
                this,
                &EqualateralTriangleWrapper::changed);
    }

    ~EqualateralTriangleWrapper() override = default;

    SD::EqualateralTriangle* raw() const { return m_ptr; }

signals:
    void changed();
};

// -------------------- Hexagon --------------------

class HexagonWrapper : public QObject {
    Q_OBJECT

    SD::Hexagon* m_ptr = nullptr;

    Q_WRITABLE_PROPERTY_CUSTOM_STORAGE(double,
                                       circumscribe_diameter,
                                       m_ptr->circumscribe_diameter);

public:
    explicit HexagonWrapper(SD::Hexagon* ptr, QObject* parent = nullptr)
        : QObject(parent), m_ptr(ptr) {
        connect(this,
                &HexagonWrapper::circumscribe_diameter_changed,
                this,
                &HexagonWrapper::changed);
    }

    ~HexagonWrapper() override = default;

    SD::Hexagon* raw() const { return m_ptr; }

signals:
    void changed();
};

// -------------------- Rectangle --------------------

class RectangleWrapper : public QObject {
    Q_OBJECT

    SD::Rectangle* m_ptr = nullptr;

    Q_WRITABLE_PROPERTY_CUSTOM_STORAGE(double, x_length, m_ptr->x_length);
    Q_WRITABLE_PROPERTY_CUSTOM_STORAGE(double, y_length, m_ptr->y_length);
    Q_WRITABLE_PROPERTY_CUSTOM_STORAGE(double, x_coord, m_ptr->x_coord);
    Q_WRITABLE_PROPERTY_CUSTOM_STORAGE(double, y_coord, m_ptr->y_coord);

public:
    explicit RectangleWrapper(SD::Rectangle* ptr, QObject* parent = nullptr)
        : QObject(parent), m_ptr(ptr) {
        connect(this,
                &RectangleWrapper::x_length_changed,
                this,
                &RectangleWrapper::changed);
        connect(this,
                &RectangleWrapper::y_length_changed,
                this,
                &RectangleWrapper::changed);
        connect(this,
                &RectangleWrapper::x_coord_changed,
                this,
                &RectangleWrapper::changed);
        connect(this,
                &RectangleWrapper::y_coord_changed,
                this,
                &RectangleWrapper::changed);
    }

    ~RectangleWrapper() override = default;

    SD::Rectangle* raw() const { return m_ptr; }

signals:
    void changed();
};

// -------------------- IrregularTriangle --------------------

class IrregularTriangleWrapper : public QObject {
    Q_OBJECT

    SD::IrregularTriangle* m_ptr = nullptr;

    Q_WRITABLE_PROPERTY_CUSTOM_STORAGE(double, x1, m_ptr->x1);
    Q_WRITABLE_PROPERTY_CUSTOM_STORAGE(double, y1, m_ptr->y1);
    Q_WRITABLE_PROPERTY_CUSTOM_STORAGE(double, x2, m_ptr->x2);
    Q_WRITABLE_PROPERTY_CUSTOM_STORAGE(double, y2, m_ptr->y2);
    Q_WRITABLE_PROPERTY_CUSTOM_STORAGE(double, x3, m_ptr->x3);
    Q_WRITABLE_PROPERTY_CUSTOM_STORAGE(double, y3, m_ptr->y3);

public:
    explicit IrregularTriangleWrapper(SD::IrregularTriangle* ptr,
                                      QObject*               parent = nullptr)
        : QObject(parent), m_ptr(ptr) {
        connect(this,
                &IrregularTriangleWrapper::x1_changed,
                this,
                &IrregularTriangleWrapper::changed);
        connect(this,
                &IrregularTriangleWrapper::y1_changed,
                this,
                &IrregularTriangleWrapper::changed);
        connect(this,
                &IrregularTriangleWrapper::x2_changed,
                this,
                &IrregularTriangleWrapper::changed);
        connect(this,
                &IrregularTriangleWrapper::y2_changed,
                this,
                &IrregularTriangleWrapper::changed);
        connect(this,
                &IrregularTriangleWrapper::x3_changed,
                this,
                &IrregularTriangleWrapper::changed);
        connect(this,
                &IrregularTriangleWrapper::y3_changed,
                this,
                &IrregularTriangleWrapper::changed);
    }

    ~IrregularTriangleWrapper() override = default;

    SD::IrregularTriangle* raw() const { return m_ptr; }

signals:
    void changed();
};

// -------------------- IrregularQuadrilateral --------------------

class IrregularQuadrilateralWrapper : public QObject {
    Q_OBJECT

    SD::IrregularQuadrilateral* m_ptr = nullptr;

    Q_WRITABLE_PROPERTY_CUSTOM_STORAGE(double, x1, m_ptr->x1);
    Q_WRITABLE_PROPERTY_CUSTOM_STORAGE(double, y1, m_ptr->y1);
    Q_WRITABLE_PROPERTY_CUSTOM_STORAGE(double, x2, m_ptr->x2);
    Q_WRITABLE_PROPERTY_CUSTOM_STORAGE(double, y2, m_ptr->y2);
    Q_WRITABLE_PROPERTY_CUSTOM_STORAGE(double, x3, m_ptr->x3);
    Q_WRITABLE_PROPERTY_CUSTOM_STORAGE(double, y3, m_ptr->y3);
    Q_WRITABLE_PROPERTY_CUSTOM_STORAGE(double, x4, m_ptr->x4);
    Q_WRITABLE_PROPERTY_CUSTOM_STORAGE(double, y4, m_ptr->y4);

public:
    explicit IrregularQuadrilateralWrapper(SD::IrregularQuadrilateral* ptr,
                                           QObject* parent = nullptr)
        : QObject(parent), m_ptr(ptr) {
        connect(this,
                &IrregularQuadrilateralWrapper::x1_changed,
                this,
                &IrregularQuadrilateralWrapper::changed);
        connect(this,
                &IrregularQuadrilateralWrapper::y1_changed,
                this,
                &IrregularQuadrilateralWrapper::changed);
        connect(this,
                &IrregularQuadrilateralWrapper::x2_changed,
                this,
                &IrregularQuadrilateralWrapper::changed);
        connect(this,
                &IrregularQuadrilateralWrapper::y2_changed,
                this,
                &IrregularQuadrilateralWrapper::changed);
        connect(this,
                &IrregularQuadrilateralWrapper::x3_changed,
                this,
                &IrregularQuadrilateralWrapper::changed);
        connect(this,
                &IrregularQuadrilateralWrapper::y3_changed,
                this,
                &IrregularQuadrilateralWrapper::changed);
        connect(this,
                &IrregularQuadrilateralWrapper::x4_changed,
                this,
                &IrregularQuadrilateralWrapper::changed);
        connect(this,
                &IrregularQuadrilateralWrapper::y4_changed,
                this,
                &IrregularQuadrilateralWrapper::changed);
    }

    ~IrregularQuadrilateralWrapper() override = default;

    SD::IrregularQuadrilateral* raw() const { return m_ptr; }

signals:
    void changed();
};

} // namespace db
