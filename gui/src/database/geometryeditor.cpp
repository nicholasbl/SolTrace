#include "geometryeditor.h"

#include "database/apertureeditor.h"
#include "database/components.h"
#include "database/surface.h"
#include <cmath>

namespace db {

SurfaceGeometry::SurfaceGeometry() {
    rebuild_geometry();
}

void SurfaceGeometry::set_new_database_connections(Database* ptr) {
    connect(ptr->geometry_parameters.self(),
            &ComponentAPIBase::changed,
            this,
            &SurfaceGeometry::parameters_changed);
}

void SurfaceGeometry::set(Database* ptr, entt::entity group) {
    observe(ptr);
    m_current_group = group;

    rebuild_geometry();
}

void SurfaceGeometry::parameters_changed(entt::entity group) {
    if (group == m_current_group) rebuild_geometry();
}

void SurfaceGeometry::rebuild_geometry() {
    clear();

    if (!database()) return;

    auto ptr = database()->geometry_parameters.get(m_current_group);

    if (!ptr) return;

    if (!ptr->surface || !ptr->aperture) {
        update();
        return;
    }

    auto surface  = ptr->surface;
    auto aperture = ptr->aperture;

    auto mesh = generate_surface(surface, aperture);
    if (!mesh || mesh->vertex.empty() || mesh->index.empty()) {
        qWarning() << "Geometry is empty, or unable to be generated";
        update();
        return;
    }

    constexpr float max_float = std::numeric_limits<float>::max();

    glm::vec3 bounds_min(max_float);
    glm::vec3 bounds_max(-max_float);

    for (auto const& p : mesh->vertex) {
        bounds_min = glm::min(bounds_min, p.position);
        bounds_max = glm::max(bounds_max, p.position);
    }

    auto indexBuffer =
        QByteArray(reinterpret_cast<const char*>(mesh->index.data()),
                   mesh->index.size() * sizeof(uint32_t));
    auto vertexBuffer =
        QByteArray(reinterpret_cast<const char*>(mesh->vertex.data()),
                   mesh->vertex.size() * sizeof(Vertex));

    addAttribute(QQuick3DGeometry::Attribute::PositionSemantic,
                 offsetof(Vertex, position),
                 QQuick3DGeometry::Attribute::ComponentType::F32Type);

    addAttribute(QQuick3DGeometry::Attribute::NormalSemantic,
                 offsetof(Vertex, normal),
                 QQuick3DGeometry::Attribute::ComponentType::F32Type);

    addAttribute(QQuick3DGeometry::Attribute::TexCoordSemantic,
                 offsetof(Vertex, uv),
                 QQuick3DGeometry::Attribute::ComponentType::F32Type);

    addAttribute(QQuick3DGeometry::Attribute::IndexSemantic,
                 0,
                 QQuick3DGeometry::Attribute::ComponentType::U32Type);

    auto bb = BoundingBox {
        .min = QVector3D(bounds_min.x, bounds_min.y, bounds_min.z),
        .max = QVector3D(bounds_max.x, bounds_max.y, bounds_max.z),
    };

    setStride(sizeof(Vertex));
    setVertexData(vertexBuffer);
    setIndexData(indexBuffer);
    setBounds(bb.min, bb.max);
    setPrimitiveType(QQuick3DGeometry::PrimitiveType::Triangles);

    set_bounding_box(bb);

    // qDebug() << Q_FUNC_INFO << entt::to_integral(m_current_group)
    //          << verts.size() << indices.size();
    // qDebug() << Q_FUNC_INFO << boundsMin << boundsMax;
    //  qDebug() << verts;

    update();
}

void SurfaceGeometry::debug() {
    qDebug() << this->bounding_box().min << this->bounding_box().max;
}

// -------------------- GroupEditor --------------------

using SurfaceEditor = GeometryEditor;

inline SD::SurfaceType string_to_surface(QString str) {
    auto string = str.toStdString();

    auto ret = reverse_lookup(SD::SurfaceTypeMap, string);

    if (ret) return *ret;
    return SD::SurfaceType::SURFACE_UNKNOWN;
}

inline SD::ApertureType string_to_aperture(QString str) {
    auto string = str.toStdString();

    auto ret = reverse_lookup(SD::ApertureTypeMap, string);

    if (ret) return *ret;
    return SD::ApertureType::APERTURE_UNKNOWN;
}

// todo: Improve
template <class K>
void build_options(QStringListModel&               dest,
                   std::map<K, std::string> const& opts) {
    QStringList items;

    for (auto const& iter : opts) {
        auto val = QString::fromStdString(iter.second);

        items.push_back(val);
    }

    dest.setStringList(items);
}


/// Twice signed area of a triangle in XY; absolute value is proportional to area.
double tri_area2(double x1,
                 double y1,
                 double x2,
                 double y2,
                 double x3,
                 double y3) {
    return (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1);
}

/// Segment intersection test used to reject self-crossing quadrilaterals.
bool segments_intersect(double ax,
                        double ay,
                        double bx,
                        double by,
                        double cx,
                        double cy,
                        double dx,
                        double dy) {
    auto orient = [](double px,
                     double py,
                     double qx,
                     double qy,
                     double rx,
                     double ry) {
        return tri_area2(px, py, qx, qy, rx, ry);
    };

    auto on_seg = [](double px,
                     double py,
                     double qx,
                     double qy,
                     double rx,
                     double ry) {
        return std::min(px, qx) <= rx && rx <= std::max(px, qx) &&
               std::min(py, qy) <= ry && ry <= std::max(py, qy);
    };

    double o1 = orient(ax, ay, bx, by, cx, cy);
    double o2 = orient(ax, ay, bx, by, dx, dy);
    double o3 = orient(cx, cy, dx, dy, ax, ay);
    double o4 = orient(cx, cy, dx, dy, bx, by);

    if ((o1 > 0 && o2 < 0 || o1 < 0 && o2 > 0) &&
        (o3 > 0 && o4 < 0 || o3 < 0 && o4 > 0)) {
        return true;
    }

    constexpr double eps = 1e-12;
    if (std::abs(o1) < eps && on_seg(ax, ay, bx, by, cx, cy)) return true;
    if (std::abs(o2) < eps && on_seg(ax, ay, bx, by, dx, dy)) return true;
    if (std::abs(o3) < eps && on_seg(cx, cy, dx, dy, ax, ay)) return true;
    if (std::abs(o4) < eps && on_seg(cx, cy, dx, dy, bx, by)) return true;

    return false;
}

/// Aperture-only validity checks (shape parameters and basic geometric sanity).
GeometryEditor::GeometryValidationStatus
validate_aperture(SD::aperture_ptr const& aperture) {
    using Status = GeometryEditor::GeometryValidationStatus;

    if (!aperture) return Status::Error;

    switch (aperture->my_type) {
    case SD::ANNULUS: {
        auto* a = dynamic_cast<SD::Annulus const*>(aperture.get());
        if (!a) return Status::Error;
        if (!std::isfinite(a->inner_radius) ||
            !std::isfinite(a->outer_radius) || !std::isfinite(a->arc_angle)) {
            return Status::Error;
        }
        if (a->inner_radius < 0 || a->outer_radius <= 0 ||
            a->inner_radius >= a->outer_radius || a->arc_angle <= 0 ||
            a->arc_angle > 2 * M_PI) {
            return Status::Error;
        }
        return Status::Ok;
    }
    case SD::CIRCLE: {
        auto* a = dynamic_cast<SD::Circle const*>(aperture.get());
        if (!a) return Status::Error;
        return (std::isfinite(a->diameter) && a->diameter > 0) ? Status::Ok
                                                               : Status::Error;
    }
    case SD::HEXAGON: {
        auto* a = dynamic_cast<SD::Hexagon const*>(aperture.get());
        if (!a) return Status::Error;
        return (std::isfinite(a->circumscribe_diameter) &&
                a->circumscribe_diameter > 0)
                   ? Status::Ok
                   : Status::Error;
    }
    case SD::RECTANGLE: {
        auto* a = dynamic_cast<SD::Rectangle const*>(aperture.get());
        if (!a) return Status::Error;
        return (std::isfinite(a->x_coord()) && std::isfinite(a->y_coord()) &&
                std::isfinite(a->x_length()) && std::isfinite(a->y_length()) &&
                a->x_length() > 0 && a->y_length() > 0)
                   ? Status::Ok
                   : Status::Error;
    }
    case SD::EQUILATERAL_TRIANGLE: {
        auto* a = dynamic_cast<SD::EqualateralTriangle const*>(aperture.get());
        if (!a) return Status::Error;
        return (std::isfinite(a->circumscribe_diameter) &&
                a->circumscribe_diameter > 0)
                   ? Status::Ok
                   : Status::Error;
    }
    case SD::IRREGULAR_TRIANGLE: {
        auto* a = dynamic_cast<SD::IrregularTriangle const*>(aperture.get());
        if (!a) return Status::Error;
        if (!std::isfinite(a->x1) || !std::isfinite(a->y1) ||
            !std::isfinite(a->x2) || !std::isfinite(a->y2) ||
            !std::isfinite(a->x3) || !std::isfinite(a->y3)) {
            return Status::Error;
        }
        return std::abs(tri_area2(a->x1, a->y1, a->x2, a->y2, a->x3, a->y3)) >
                       1e-10
                   ? Status::Ok
                   : Status::Error;
    }
    case SD::IRREGULAR_QUADRILATERAL: {
        auto* a = dynamic_cast<SD::IrregularQuadrilateral const*>(aperture.get());
        if (!a) return Status::Error;
        if (!std::isfinite(a->x1) || !std::isfinite(a->y1) ||
            !std::isfinite(a->x2) || !std::isfinite(a->y2) ||
            !std::isfinite(a->x3) || !std::isfinite(a->y3) ||
            !std::isfinite(a->x4) || !std::isfinite(a->y4)) {
            return Status::Error;
        }

        // Treat bow-tie quadrilaterals as invalid.
        if (segments_intersect(a->x1, a->y1, a->x2, a->y2, a->x3, a->y3, a->x4, a->y4) ||
            segments_intersect(a->x2, a->y2, a->x3, a->y3, a->x4, a->y4, a->x1, a->y1)) {
            return Status::Error;
        }

        double area2 = tri_area2(a->x1, a->y1, a->x2, a->y2, a->x3, a->y3) +
                       tri_area2(a->x1, a->y1, a->x3, a->y3, a->x4, a->y4);
        return std::abs(area2) > 1e-10 ? Status::Ok : Status::Error;
    }
    case SD::SINGLE_AXIS_CURVATURE_SECTION: return Status::Warning;
    case SD::APERTURE_UNKNOWN: return Status::Error;
    }

    return Status::Error;
}

/// Surface-only validity checks (parameter ranges and support level).
GeometryEditor::GeometryValidationStatus
validate_surface(SD::surface_ptr const& surface) {
    using Status = GeometryEditor::GeometryValidationStatus;

    if (!surface) return Status::Error;

    switch (surface->my_type) {
    case SD::FLAT: return Status::Ok;
    case SD::PARABOLA: {
        auto* s = dynamic_cast<SD::Parabola const*>(surface.get());
        if (!s) return Status::Error;
        if (!std::isfinite(s->focal_length_x) ||
            !std::isfinite(s->focal_length_y)) {
            return Status::Error;
        }
        if (s->focal_length_x == 0 || s->focal_length_y == 0) {
            return Status::Error;
        }
        if (s->focal_length_x < 0 || s->focal_length_y < 0) {
            return Status::Warning;
        }
        return Status::Ok;
    }
    case SD::CONE: {
        auto* s = dynamic_cast<SD::Cone const*>(surface.get());
        if (!s) return Status::Error;
        if (!std::isfinite(s->half_angle)) return Status::Error;
        if (s->half_angle <= 0 || s->half_angle >= (M_PI / 2.0)) {
            return Status::Error;
        }
        return Status::Ok;
    }
    case SD::CYLINDER: {
        auto* s = dynamic_cast<SD::Cylinder const*>(surface.get());
        if (!s) return Status::Error;
        if (!std::isfinite(s->radius)) return Status::Error;
        if (s->radius <= 0) return Status::Error;
        return Status::Ok;
    }
    case SD::SPHERE: {
        auto* s = dynamic_cast<SD::Sphere const*>(surface.get());
        if (!s) return Status::Error;
        if (!std::isfinite(s->vertex_curv)) return Status::Error;
        return s->vertex_curv == 0 ? Status::Warning : Status::Ok;
    }
    case SD::TORUS:
    case SD::HYPER:
    case SD::GENERAL_SPENCER_MURTY: return Status::Warning;
    case SD::SURFACE_UNKNOWN: return Status::Error;
    }

    return Status::Error;
}

/// Pairwise compatibility check:
/// confirms the selected surface can evaluate z(x,y) across aperture samples.
GeometryEditor::GeometryValidationStatus
validate_surface_aperture_pair(SD::surface_ptr const&  surface,
                               SD::aperture_ptr const& aperture) {
    using Status = GeometryEditor::GeometryValidationStatus;

    if (!surface || !aperture) return Status::Error;

    auto [points, indices] = aperture->triangulation();
    Q_UNUSED(indices)

    if (points.size() < 2) return Status::Error;

    // Sample the full set for smaller apertures, stride for larger meshes.
    int sample_points = points.size() / 2;
    int stride        = std::max(1, sample_points / 64);

    for (int i = 0; i < sample_points; i += stride) {
        double x = points[2 * i];
        double y = points[2 * i + 1];
        double z = surface->z(x, y);
        if (!std::isfinite(x) || !std::isfinite(y) || !std::isfinite(z))
            return Status::Error;
    }

    return Status::Ok;
}

GeometryEditor::GeometryEditor(QObject* parent)
    : QObject { parent },
      m_surface_geometry(new SurfaceGeometry()),
      m_surface_type_model(new QStringListModel(this)),
      m_aperture_type_model(new QStringListModel(this)),
      m_aperture_parameter_model(new ApertureParameterModel(this)),
      m_surface_parameter_model(new SurfaceParameterModel(this))

{

    build_options(*m_surface_type_model, SD::SurfaceTypeMap);
    build_options(*m_aperture_type_model, SD::ApertureTypeMap);

    connect(this,
            &GeometryEditor::surface_kind_changed,
            this,
            &GeometryEditor::updated);

    connect(
        this, &GeometryEditor::kind_changed, this, &GeometryEditor::updated);

    connect(m_surface_parameter_model,
            &SurfaceParameterModel::updated,
            this,
            [this]() {
                if (!database()) return;
                database()->as_registry().patch<GeometryComponent>(
                    m_current_group,
                    [this](GeometryComponent& params) {
                        if (params.surface) {
                            m_surface_parameter_model->write_back(*params.surface);
                        }
                    });
                emit updated();
            });

    connect(m_aperture_parameter_model,
            &ApertureParameterModel::updated,
            this,
            [this]() {
                if (!database()) return;
                database()->as_registry().patch<GeometryComponent>(
                    m_current_group, [this](GeometryComponent& params) {
                        if (params.aperture) {
                            m_aperture_parameter_model->write_back(
                                *params.aperture);
                        }
                    });
                emit updated();
            });

    connect(this,
            &GeometryEditor::updated,
            this,
            &GeometryEditor::evaluate_geometry_validation);

    make_new_aperture(SolTrace::Data::APERTURE_UNKNOWN);
}

GeometryEditor::~GeometryEditor() {
    delete m_surface_geometry;
}

void GeometryEditor::parameters_changed(entt::entity e) {
    if (this->m_current_group != e) return;
    if (auto* params = database()->geometry_parameters.get(m_current_group)) {
        if (params->surface && params->aperture) {
            m_surface_parameter_model->set_from(*params->surface);
            m_aperture_parameter_model->set_from(*params->aperture);
        } else {
            m_surface_parameter_model->set_for(SD::FLAT);
            m_aperture_parameter_model->set_for(SD::ApertureType::RECTANGLE);
        }
    }
    emit surface_kind_changed();
    emit kind_changed();
}

void GeometryEditor::set_new_database_connections(Database* ptr) {
    add_connection(connect(ptr->geometry_parameters.self(),
                           &ComponentAPIBase::changed,
                           this,
                           &GeometryEditor::parameters_changed));
}

void GeometryEditor::make_new_aperture(SD::ApertureType type) {
    if (!database()) return;

    m_aperture_parameter_model->set_for(type);
}

void GeometryEditor::make_new_surface(SD::SurfaceType type) {
    if (!database()) return;

    m_surface_parameter_model->set_for(type);
}

void GeometryEditor::set(Database* database, entt::entity group) {
    observe(database);
    m_current_group = group;
    m_surface_geometry->set(database, group);
    if (database) {
        if (auto* params = database->geometry_parameters.get(group)) {
            if (params->surface && params->aperture) {
                m_aperture_parameter_model->set_from(*params->aperture);
                m_surface_parameter_model->set_from(*params->surface);
            } else {
                m_surface_parameter_model->set_for(SD::FLAT);
                m_aperture_parameter_model->set_for(
                    SD::ApertureType::RECTANGLE);
            }
        }
    }
    emit updated();
}

QString GeometryEditor::kind() const {
    return m_kind;
}

void GeometryEditor::set_kind(QString newKind) {
    if (m_kind == newKind) return;
    m_kind = newKind;
    make_new_aperture(string_to_aperture(m_kind));
    emit kind_changed();
}

QString GeometryEditor::surface_kind() const {
    return m_surf_kind;
}

void GeometryEditor::set_surface_kind(QString newSurface_kind) {
    if (m_surf_kind == newSurface_kind) return;
    m_surf_kind = newSurface_kind;
    m_surface_parameter_model->set_for(string_to_surface(m_surf_kind));
    make_new_surface(string_to_surface(m_surf_kind));
    emit surface_kind_changed();
}

GeometryEditor::GeometryValidationStatus
GeometryEditor::geometry_validation_status() const {
    return m_geometry_validation_status;
}

/// Collapses all geometry checks into a single severity for UI consumption.
void GeometryEditor::evaluate_geometry_validation() {
    using Status = GeometryEditor::GeometryValidationStatus;

    auto merge = [](Status a, Status b) {
        auto rank = [](Status s) {
            switch (s) {
            case Status::Ok: return 0;
            case Status::Warning: return 1;
            case Status::Error: return 2;
            }
            return 2;
        };
        return rank(b) > rank(a) ? b : a;
    };

    Status new_status = Status::Error;

    if (database()) {
        auto* params = database()->geometry_parameters.get(m_current_group);
        if (params) {
            new_status = Status::Ok;
            new_status = merge(new_status, validate_aperture(params->aperture));
            new_status = merge(new_status, validate_surface(params->surface));
            new_status =
                merge(new_status,
                      validate_surface_aperture_pair(params->surface,
                                                     params->aperture));
        }
    }

    if (new_status == m_geometry_validation_status) return;
    m_geometry_validation_status = new_status;
    emit geometry_validation_status_changed();
}

} // namespace db
