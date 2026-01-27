#include "geometryeditor.h"

#include "database/components.h"

namespace db {

SurfaceGeometry::SurfaceGeometry() {
    rebuild_geometry();
}

void SurfaceGeometry::set_new_database_connections(Database* ptr) {
    connect(ptr->group_parameters.self(),
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

    auto ptr = database()->group_parameters.get(m_current_group);

    if (!ptr) return;

    if (!ptr->surface || !ptr->aperture) {
        update();
        return;
    }

    auto surface  = ptr->surface;
    auto aperture = ptr->aperture;

    auto [points, indices] = aperture->triangulation();

    QVector<Vertex> verts(points.size() / 2);

    for (auto& v : verts) {
        v.normal = QVector3D(0, 0, 0);
        v.uv     = QVector2D(0, 0);
    }


    constexpr float max_float = std::numeric_limits<float>::max();

    QVector3D boundsMin(max_float, max_float, max_float);
    QVector3D boundsMax(-max_float, -max_float, -max_float);

    // Compute the positions
    for (int i = 0; i < verts.size(); ++i) {
        auto x        = points[2 * i];
        auto y        = points[2 * i + 1];
        auto position = QVector3D(x, y, surface->z(x, y));

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

        Q_ASSERT(a < verts.size() && b < verts.size() && c < verts.size());

        QVector3D normal =
            QVector3D::crossProduct(verts[b].position - verts[a].position,
                                    verts[c].position - verts[a].position);

        verts[a].normal += normal;
        verts[b].normal += normal;
        verts[c].normal += normal;
    }

    // Compute uv and normalize
    for (int i = 0; i < verts.size(); ++i) {

        verts[i].normal.normalize();

        float dx = boundsMax.x() - boundsMin.x();
        float dy = boundsMax.y() - boundsMin.y();
        if (dx == 0) dx = 1;
        if (dy == 0) dy = 1;

        verts[i].uv.setX((verts[i].position.x() - boundsMin.x()) / dx);
        verts[i].uv.setY((verts[i].position.y() - boundsMin.y()) / dy);
    }

    auto indexBuffer = QByteArray(reinterpret_cast<const char*>(indices.data()),
                                  indices.size() * sizeof(int));
    auto vertexBuffer = QByteArray(reinterpret_cast<const char*>(verts.data()),
                                   verts.size() * sizeof(Vertex));

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

    setStride(sizeof(Vertex));
    setVertexData(vertexBuffer);
    setIndexData(indexBuffer);
    setBounds(boundsMin, boundsMax);
    setPrimitiveType(QQuick3DGeometry::PrimitiveType::Triangles);

    qDebug() << Q_FUNC_INFO << entt::to_integral(m_current_group)
             << verts.size() << indices.size();

    update();
}

// -------------------- GroupEditor --------------------

using SurfaceEditor = GroupEditor;

static SD::SurfaceType convert(GroupEditor::SurfaceKind k) {
    switch (k) {
    case GroupEditor::SurfaceKind::Cone: return SD::SurfaceType::CONE;
    case GroupEditor::SurfaceKind::Cylinder: return SD::SurfaceType::CYLINDER;
    case GroupEditor::SurfaceKind::Flat: return SD::SurfaceType::FLAT;
    case GroupEditor::SurfaceKind::Parabola: return SD::SurfaceType::PARABOLA;
    case GroupEditor::SurfaceKind::Sphere: return SD::SurfaceType::SPHERE;
    default: return SD::SurfaceType::SURFACE_UNKNOWN;
    }
}

static GroupEditor::SurfaceKind convert(SD::SurfaceType k) {
    switch (k) {
    case SolTrace::Data::CONE: return GroupEditor::SurfaceKind::Cone;
    case SolTrace::Data::CYLINDER: return GroupEditor::SurfaceKind::Cylinder;
    case SolTrace::Data::FLAT: return GroupEditor::SurfaceKind::Flat;
    case SolTrace::Data::PARABOLA: return GroupEditor::SurfaceKind::Parabola;
    case SolTrace::Data::SPHERE: return GroupEditor::SurfaceKind::Sphere;
    default: return GroupEditor::SurfaceKind::Unknown;
    }
}

static SD::ApertureType convert(GroupEditor::ApertureKind k) {
    switch (k) {
    case GroupEditor::ApertureKind::Annulus: return SD::ApertureType::ANNULUS;
    case GroupEditor::ApertureKind::Circle: return SD::ApertureType::CIRCLE;
    case GroupEditor::ApertureKind::Hexagon: return SD::ApertureType::HEXAGON;
    case GroupEditor::ApertureKind::Rectangle:
        return SD::ApertureType::RECTANGLE;
    case GroupEditor::ApertureKind::Equilateral_Triangle:
        return SD::ApertureType::EQUILATERAL_TRIANGLE;
    case GroupEditor::ApertureKind::Single_Axis_Curvature_Section:
        return SD::ApertureType::SINGLE_AXIS_CURVATURE_SECTION;
    case GroupEditor::ApertureKind::Irregular_Triangle:
        return SD::ApertureType::IRREGULAR_TRIANGLE;
    case GroupEditor::ApertureKind::Irregular_Quadrilateral:
        return SD::ApertureType::IRREGULAR_QUADRILATERAL;
    default: return SD::ApertureType::APERTURE_UNKNOWN;
    }
}

static GroupEditor::ApertureKind convert(SD::ApertureType k) {
    switch (k) {
    case SolTrace::Data::ANNULUS: return GroupEditor::ApertureKind::Annulus;
    case SolTrace::Data::CIRCLE: return GroupEditor::ApertureKind::Circle;
    case SolTrace::Data::HEXAGON: return GroupEditor::ApertureKind::Hexagon;
    case SolTrace::Data::RECTANGLE: return GroupEditor::ApertureKind::Rectangle;
    case SolTrace::Data::EQUILATERAL_TRIANGLE:
        return GroupEditor::ApertureKind::Equilateral_Triangle;
    case SolTrace::Data::SINGLE_AXIS_CURVATURE_SECTION:
        return GroupEditor::ApertureKind::Single_Axis_Curvature_Section;
    case SolTrace::Data::IRREGULAR_TRIANGLE:
        return GroupEditor::ApertureKind::Irregular_Triangle;
    case SolTrace::Data::IRREGULAR_QUADRILATERAL:
        return GroupEditor::ApertureKind::Irregular_Quadrilateral;
    default: return GroupEditor::ApertureKind::Unknown;
    }
}

GroupEditor::GroupEditor(QObject* parent)
    : QObject { parent }, m_surface_geometry(new SurfaceGeometry()) {


    connect(
        this, &GroupEditor::surface_kind_changed, this, &GroupEditor::updated);

    connect(this, &GroupEditor::kind_changed, this, &GroupEditor::updated);

    connect(this, &GroupEditor::surface_arguments_changed, this, [this]() {
        make_new_surface(m_surf_kind);
    });

    make_new_aperture(ApertureKind::Rectangle);
}

GroupEditor::~GroupEditor() {
    delete m_surface_geometry;
}

void GroupEditor::parameters_changed(entt::entity e) { }

void GroupEditor::set_new_database_connections(Database* ptr) {
    connect(ptr->group_parameters.self(),
            &ComponentAPIBase::changed,
            this,
            &GroupEditor::parameters_changed);
}

void GroupEditor::make_new_aperture(ApertureKind type) {
    if (!database()) return;

    if (m_aperture_editor) {
        QObject* p = m_aperture_editor;
        set_aperture_editor(nullptr);
        delete p;
    }

#define EDIT_CASE(TYPE, ...)                                                   \
    {                                                                          \
        auto ptr        = SD::make_aperture<SD::TYPE>(__VA_ARGS__);            \
        params.aperture = ptr;                                                 \
        auto editor     = new TYPE##Wrapper(ptr.get(), this);                  \
        set_aperture_editor(editor);                                           \
        connect(editor, &TYPE##Wrapper::changed, this, &GroupEditor::updated); \
    }                                                                          \
    break;

    database()->as_registry().patch<GroupParameterComponent>(
        m_current_group, [this, type](GroupParameterComponent& params) {
            switch (convert(type)) {
            case SolTrace::Data::ANNULUS:
                EDIT_CASE(Annulus, 0.0, 1.0, 2 * M_PI);
            case SolTrace::Data::CIRCLE: EDIT_CASE(Circle, 1.0);
            case SolTrace::Data::HEXAGON: EDIT_CASE(Hexagon, 1.0);
            case SolTrace::Data::RECTANGLE: EDIT_CASE(Rectangle, 1.0, 1.0);
            case SolTrace::Data::EQUILATERAL_TRIANGLE:
                EDIT_CASE(EqualateralTriangle, 1.0);
            case SolTrace::Data::IRREGULAR_TRIANGLE:
                EDIT_CASE(IrregularTriangle, 0, 1, 0, 0, 1, 0);
            case SolTrace::Data::IRREGULAR_QUADRILATERAL:
                EDIT_CASE(IrregularQuadrilateral, -1, -1, -1, 1, 1, 1, 1, -1);
            default: return make_new_aperture(ApertureKind::Circle);
            }
        });
}

void GroupEditor::make_new_surface(SurfaceKind type) {
    // weeeee
    auto const& l      = surface_arguments();
    auto        sdvec  = std::vector<double> { l.begin(), l.end() };
    database()->as_registry().patch<GroupParameterComponent>(
        m_current_group, [sdvec, type](GroupParameterComponent& params) {
            params.surface = SD::make_surface_from_type(convert(type), sdvec);
        });
}

void GroupEditor::set(Database* database, entt::entity group) {
    observe(database);
    m_current_group = group;
    m_surface_geometry->set(database, group);
    emit updated();
}

GroupEditor::ApertureKind GroupEditor::kind() const {
    return m_kind;
}

void GroupEditor::set_kind(ApertureKind newKind) {
    if (m_kind == newKind) return;
    m_kind = newKind;
    make_new_aperture(m_kind);
    emit kind_changed();
}

GroupEditor::SurfaceKind GroupEditor::surface_kind() const {
    return m_surf_kind;
}

void GroupEditor::set_surface_kind(SurfaceKind newSurface_kind) {
    if (m_surf_kind == newSurface_kind) return;
    m_surf_kind = newSurface_kind;
    make_new_surface(m_surf_kind);
    emit surface_kind_changed();
}

} // namespace db
