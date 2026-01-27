#include "components.h"
#include <QtGui/qmatrix4x4.h>

// TODO: Consolidate

namespace SolTrace {
namespace Data {
bool operator==(SD::OpticalProperties const& a,
                SD::OpticalProperties const& b) {
    return std::tie(a.my_type,
                    a.error_distribution_type,
                    a.transmitivity,
                    a.reflectivity,
                    a.slope_error,
                    a.specularity_error,
                    a.refraction_index_front,
                    a.refraction_index_back) ==
           std::tie(b.my_type,
                    b.error_distribution_type,
                    b.transmitivity,
                    b.reflectivity,
                    b.slope_error,
                    b.specularity_error,
                    b.refraction_index_front,
                    b.refraction_index_back);
}
} // namespace Data
} // namespace SolTrace


namespace db {

QMatrix4x4 TransformComponent::as_matrix() const {
    QMatrix4x4 m;
    m.setToIdentity();
    m.translate(position);
    m.rotate(rotation);
    return m;
}

// --- Aperture value equality -----------------------------------------------

static bool operator==(SD::Annulus const& a, SD::Annulus const& b) {
    return std::tie(a.inner_radius, a.outer_radius, a.arc_angle) ==
           std::tie(b.inner_radius, b.outer_radius, b.arc_angle);
}
static bool operator==(SD::Circle const& a, SD::Circle const& b) {
    return a.diameter == b.diameter;
}
static bool operator==(SD::Hexagon const& a, SD::Hexagon const& b) {
    return a.circumscribe_diameter == b.circumscribe_diameter;
}
static bool operator==(SD::Rectangle const& a, SD::Rectangle const& b) {
    return std::tie(a.x_length, a.y_length, a.x_coord, a.y_coord) ==
           std::tie(b.x_length, b.y_length, b.x_coord, b.y_coord);
}
static bool operator==(SD::EqualateralTriangle const& a,
                       SD::EqualateralTriangle const& b) {
    return a.circumscribe_diameter == b.circumscribe_diameter;
}
static bool operator==(SD::IrregularTriangle const& a,
                       SD::IrregularTriangle const& b) {
    return std::tie(a.x1, a.y1, a.x2, a.y2, a.x3, a.y3) ==
           std::tie(b.x1, b.y1, b.x2, b.y2, b.x3, b.y3);
}
static bool operator==(SD::IrregularQuadrilateral const& a,
                       SD::IrregularQuadrilateral const& b) {
    return std::tie(a.x1, a.y1, a.x2, a.y2, a.x3, a.y3, a.x4, a.y4) ==
           std::tie(b.x1, b.y1, b.x2, b.y2, b.x3, b.y3, b.x4, b.y4);
}

static bool is_equal(SD::Aperture const& a, SD::Aperture const& b) {
    if (a.my_type != b.my_type) return false;

    switch (a.my_type) {

    case SolTrace::Data::ANNULUS:
        return *dynamic_cast<SD::Annulus const*>(&a) ==
               *dynamic_cast<SD::Annulus const*>(&b);
    case SolTrace::Data::CIRCLE:
        return *dynamic_cast<SD::Circle const*>(&a) ==
               *dynamic_cast<SD::Circle const*>(&b);
    case SolTrace::Data::HEXAGON:
        return *dynamic_cast<SD::Hexagon const*>(&a) ==
               *dynamic_cast<SD::Hexagon const*>(&b);
    case SolTrace::Data::RECTANGLE:
        return *dynamic_cast<SD::Rectangle const*>(&a) ==
               *dynamic_cast<SD::Rectangle const*>(&b);
    case SolTrace::Data::EQUILATERAL_TRIANGLE:
        return *dynamic_cast<SD::EqualateralTriangle const*>(&a) ==
               *dynamic_cast<SD::EqualateralTriangle const*>(&b);
    case SolTrace::Data::SINGLE_AXIS_CURVATURE_SECTION:
        // TODO: When that class is implemented, we can implement this
        return false;
    case SolTrace::Data::IRREGULAR_TRIANGLE:
        return *dynamic_cast<SD::IrregularTriangle const*>(&a) ==
               *dynamic_cast<SD::IrregularTriangle const*>(&b);
    case SolTrace::Data::IRREGULAR_QUADRILATERAL:
        return *dynamic_cast<SD::IrregularQuadrilateral const*>(&a) ==
               *dynamic_cast<SD::IrregularQuadrilateral const*>(&b);
    case SolTrace::Data::APERTURE_UNKNOWN: return false;
    }
}

static bool is_equal(SD::aperture_ptr const& a, SD::aperture_ptr const& b) {
    if (!a || !b) return a == b;
    return is_equal(*a, *b);
}

// --- Surface value equality -------------------------------------------------

static bool operator==(SD::Cone const& a, SD::Cone const& b) {
    return a.half_angle == b.half_angle;
}
static bool operator==(SD::Cylinder const& a, SD::Cylinder const& b) {
    return a.radius == b.radius;
}
static bool operator==(SD::Flat const&, SD::Flat const&) {
    return true;
}
static bool operator==(SD::Parabola const& a, SD::Parabola const& b) {
    return std::tie(a.focal_length_x, a.focal_length_y) ==
           std::tie(b.focal_length_x, b.focal_length_y);
}
static bool operator==(SD::Sphere const& a, SD::Sphere const& b) {
    return a.vertex_curv == b.vertex_curv;
}

static bool is_equal(SD::Surface const& a, SD::Surface const& b) {
    if (a.my_type != b.my_type) return false;

    switch (a.my_type) {
    case SolTrace::Data::CONE:
        return *dynamic_cast<SD::Cone const*>(&a) ==
               *dynamic_cast<SD::Cone const*>(&b);
    case SolTrace::Data::CYLINDER:
        return *dynamic_cast<SD::Cylinder const*>(&a) ==
               *dynamic_cast<SD::Cylinder const*>(&b);
    case SolTrace::Data::FLAT:
        return *dynamic_cast<SD::Flat const*>(&a) ==
               *dynamic_cast<SD::Flat const*>(&b);
    case SolTrace::Data::PARABOLA:
        return *dynamic_cast<SD::Parabola const*>(&a) ==
               *dynamic_cast<SD::Parabola const*>(&b);
    case SolTrace::Data::SPHERE:
        return *dynamic_cast<SD::Sphere const*>(&a) ==
               *dynamic_cast<SD::Sphere const*>(&b);
    case SolTrace::Data::HYPER:
    case SolTrace::Data::GENERAL_SPENCER_MURTY:
    case SolTrace::Data::TORUS:
    case SolTrace::Data::SURFACE_UNKNOWN: return false;
    }
}

static bool is_equal(SD::surface_ptr const& a, SD::surface_ptr const& b) {
    if (!a || !b) return a == b;
    return is_equal(*a, *b);
}

bool GroupParameters::operator==(db::GroupParameters const& b) const {
    return is_equal(aperture, b.aperture) and is_equal(surface, b.surface) and
           optics_front == b.optics_front and optics_back == b.optics_back;
}


} // namespace db
