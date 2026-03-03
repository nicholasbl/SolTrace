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

        // qDebug() << position;

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

    float dx = boundsMax.x() - boundsMin.x();
    float dy = boundsMax.y() - boundsMin.y();
    if (dx == 0) dx = 1;
    if (dy == 0) dy = 1;

    // Compute uv and normalize
    for (int i = 0; i < verts.size(); ++i) {

        auto& n = verts[i].normal;
        if (!qFuzzyIsNull(n.lengthSquared())) {
            n.normalize();
        } else {
            n = QVector3D(0, 0, 1);
        }


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

    set_bounding_box(BoundingBox {
        .min = boundsMin,
        .max = boundsMax,
    });

    qDebug() << Q_FUNC_INFO << entt::to_integral(m_current_group)
             << verts.size() << indices.size();
    qDebug() << Q_FUNC_INFO << boundsMin << boundsMax;
    // qDebug() << verts;

    update();
}

// -------------------- GroupEditor --------------------

using SurfaceEditor = GroupEditor;

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

GroupEditor::GroupEditor(QObject* parent)
    : QObject { parent },
      m_surface_geometry(new SurfaceGeometry()),
      m_back_editor(new OpticalPropertiesObject(true, this)),
      m_front_editor(new OpticalPropertiesObject(false, this)),
      m_interaction_type_model(new QStringListModel(this)),
      m_distribution_type_model(new QStringListModel(this)),
      m_surface_type_model(new QStringListModel(this)),
      m_aperture_type_model(new QStringListModel(this))

{

    build_options(*m_interaction_type_model, SD::InteractionTypeMap);
    build_options(*m_distribution_type_model, SD::DistributionTypeMap);
    build_options(*m_surface_type_model, SD::SurfaceTypeMap);
    build_options(*m_aperture_type_model, SD::ApertureTypeMap);

    connect(
        this, &GroupEditor::surface_kind_changed, this, &GroupEditor::updated);

    connect(this, &GroupEditor::kind_changed, this, &GroupEditor::updated);

    // TODO spurious rebuilds
    connect(this, &GroupEditor::surface_arguments_changed, this, [this]() {
        make_new_surface(string_to_surface(m_surf_kind));
    });

    make_new_aperture(SolTrace::Data::APERTURE_UNKNOWN);
}

GroupEditor::~GroupEditor() {
    delete m_surface_geometry;
}

void GroupEditor::parameters_changed(entt::entity e) {
    if (this->m_current_group != e) return;
    emit surface_kind_changed();
    emit kind_changed();
}

void GroupEditor::set_new_database_connections(Database* ptr) {
    add_connection(connect(ptr->group_parameters.self(),
                           &ComponentAPIBase::changed,
                           this,
                           &GroupEditor::parameters_changed));
}

void GroupEditor::make_new_aperture(SD::ApertureType type) {
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

    database()->as_registry().patch<RenderGroupParameterComponent>(
        m_current_group, [this, type](RenderGroupParameterComponent& params) {
            switch (type) {
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
            default: return make_new_aperture(SolTrace::Data::APERTURE_UNKNOWN);
            }
        });
}

void GroupEditor::make_new_surface(SD::SurfaceType type) {
    // weeeee
    auto const& l      = surface_arguments();
    auto        sdvec  = std::vector<double> { l.begin(), l.end() };
    database()->as_registry().patch<RenderGroupParameterComponent>(
        m_current_group, [sdvec, type](RenderGroupParameterComponent& params) {
            params.surface = SD::make_surface_from_type(type, sdvec);
        });
}

void GroupEditor::set(Database* database, entt::entity group) {
    observe(database);
    m_current_group = group;
    m_surface_geometry->set(database, group);
    emit updated();
}

QString GroupEditor::kind() const {
    return m_kind;
}

void GroupEditor::set_kind(QString newKind) {
    if (m_kind == newKind) return;
    m_kind = newKind;
    make_new_aperture(string_to_aperture(m_kind));
    emit kind_changed();
}

QString GroupEditor::surface_kind() const {
    return m_surf_kind;
}

void GroupEditor::set_surface_kind(QString newSurface_kind) {
    if (m_surf_kind == newSurface_kind) return;
    m_surf_kind = newSurface_kind;
    make_new_surface(string_to_surface(m_surf_kind));
    emit surface_kind_changed();
}

} // namespace db
