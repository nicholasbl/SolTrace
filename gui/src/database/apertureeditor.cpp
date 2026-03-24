#include "apertureeditor.h"

namespace db {

namespace {

QVector<ApertureParameter> make_parameters_for(SD::ApertureType type) {
    QVector<ApertureParameter> ret;

    switch (type) {
    case SolTrace::Data::ANNULUS:
        ret = {
            {
                .name        = "Inner radius",
                .content     = QPointF(0.0, 0.0),
                .min_bounded = true,
                .min         = QPointF(0.0, 0.0),
            },
            {
                .name        = "Outer radius",
                .content     = QPointF(1.0, 0.0),
                .min_bounded = true,
                .min         = QPointF(0.0, 0.0),
            },
            {
                .name        = "Arc angle",
                .content     = QPointF(2 * M_PI, 0.0),
                .min_bounded = true,
                .max_bounded = true,
                .min         = QPointF(0.0, 0.0),
                .max         = QPointF(2 * M_PI, 0.0),
            },
        };
        break;
    case SolTrace::Data::CIRCLE:
        ret = {
            {
                .name        = "Diameter",
                .content     = QPointF(1.0, 0.0),
                .min_bounded = true,
                .min         = QPointF(0.0, 0.0),
            },
        };
        break;
    case SolTrace::Data::HEXAGON:
        ret = {
            {
                .name        = "Circumscribe diameter",
                .content     = QPointF(1.0, 0.0),
                .min_bounded = true,
                .min         = QPointF(0.0, 0.0),
            },
        };
        break;
    case SolTrace::Data::RECTANGLE:
        ret = {
            { .name     = "Lower-left corner",
              .is_point = true,
              .content  = QPointF(-0.5, -0.5) },
            {
                .name        = "Size",
                .is_point    = true,
                .content     = QPointF(1.0, 1.0),
                .min_bounded = true,
                .min         = QPointF(0.0, 0.0),
            },
        };
        break;
    case SolTrace::Data::EQUILATERAL_TRIANGLE:
        ret = {
            {
                .name        = "Circumscribe diameter",
                .content     = QPointF(1.0, 0.0),
                .min_bounded = true,
                .min         = QPointF(0.0, 0.0),
            },
        };
        break;
    case SolTrace::Data::SINGLE_AXIS_CURVATURE_SECTION:
        ret = {
            { .name = "X min", .content = QPointF(-0.5, 0.0) },
            { .name = "X max", .content = QPointF(0.5, 0.0) },
            {
                .name        = "Y length",
                .content     = QPointF(1.0, 0.0),
                .min_bounded = true,
                .min         = QPointF(0.0, 0.0),
            },
        };
        break;
    case SolTrace::Data::IRREGULAR_TRIANGLE:
        ret = {
            {
                .name     = "Point 1",
                .is_point = true,
                .content  = QPointF(0.0, 1.0),
            },
            {
                .name     = "Point 2",
                .is_point = true,
                .content  = QPointF(0.0, 0.0),
            },
            {
                .name     = "Point 3",
                .is_point = true,
                .content  = QPointF(1.0, 0.0),
            },
        };
        break;
    case SolTrace::Data::IRREGULAR_QUADRILATERAL:
        ret = {
            {
                .name     = "Point 1",
                .is_point = true,
                .content  = QPointF(-1.0, -1.0),
            },
            {
                .name     = "Point 2",
                .is_point = true,
                .content  = QPointF(-1.0, 1.0),
            },
            {
                .name     = "Point 3",
                .is_point = true,
                .content  = QPointF(1.0, 1.0),
            },
            {
                .name     = "Point 4",
                .is_point = true,
                .content  = QPointF(1.0, -1.0),
            },
        };
        break;
    case SolTrace::Data::APERTURE_UNKNOWN: break;
    }

    return ret;
}

double scalar_value(ApertureParameter const& p) {
    return p.content.x();
}

QPointF point_value(ApertureParameter const& p) {
    return p.content;
}

} // namespace

ApertureParameterModel::ApertureParameterModel(QObject* parent)
    : StructTableModel(parent) {

    connect(this,
            &ApertureParameterModel::dataChanged,
            this,
            &ApertureParameterModel::updated);
}

void ApertureParameterModel::set_for(SD::ApertureType type) {
    this->reset(make_parameters_for(type));
}

void ApertureParameterModel::set_from(SD::Aperture const& aperture) {
    auto ret = make_parameters_for(aperture.my_type);

    switch (aperture.my_type) {
    case SolTrace::Data::ANNULUS: {
        auto const* ptr = dynamic_cast<SD::Annulus const*>(&aperture);
        if (!ptr) break;
        ret = {
            {
                .name        = "Inner radius",
                .content     = QPointF(ptr->inner_radius, 0.0),
                .min_bounded = true,
                .min         = QPointF(0.0, 0.0),
            },
            {
                .name        = "Outer radius",
                .content     = QPointF(ptr->outer_radius, 0.0),
                .min_bounded = true,
                .min         = QPointF(0.0, 0.0),
            },
            {
                .name        = "Arc angle",
                .content     = QPointF(ptr->arc_angle, 0.0),
                .min_bounded = true,
                .max_bounded = true,
                .min         = QPointF(0.0, 0.0),
                .max         = QPointF(2 * M_PI, 0.0),
            },
        };
        break;
    }
    case SolTrace::Data::CIRCLE: {
        auto const* ptr = dynamic_cast<SD::Circle const*>(&aperture);
        if (!ptr) break;
        ret = {
            {
                .name        = "Diameter",
                .content     = QPointF(ptr->diameter, 0.0),
                .min_bounded = true,
                .min         = QPointF(0.0, 0.0),
            },
        };
        break;
    }
    case SolTrace::Data::HEXAGON: {
        auto const* ptr = dynamic_cast<SD::Hexagon const*>(&aperture);
        if (!ptr) break;
        ret = {
            {
                .name        = "Circumscribe diameter",
                .content     = QPointF(ptr->circumscribe_diameter, 0.0),
                .min_bounded = true,
                .min         = QPointF(0.0, 0.0),
            },
        };
        break;
    }
    case SolTrace::Data::RECTANGLE: {
        auto const* ptr = dynamic_cast<SD::Rectangle const*>(&aperture);
        if (!ptr) break;
        ret = {
            { .name     = "Lower-left corner",
              .is_point = true,
              .content  = QPointF(ptr->x_coord(), ptr->y_coord()) },
            {
                .name        = "Size",
                .is_point    = true,
                .content     = QPointF(ptr->x_length(), ptr->y_length()),
                .min_bounded = true,
                .min         = QPointF(0.0, 0.0),
            },
        };
        break;
    }
    case SolTrace::Data::EQUILATERAL_TRIANGLE: {
        auto const* ptr =
            dynamic_cast<SD::EqualateralTriangle const*>(&aperture);
        if (!ptr) break;
        ret = {
            {
                .name        = "Circumscribe diameter",
                .content     = QPointF(ptr->circumscribe_diameter, 0.0),
                .min_bounded = true,
                .min         = QPointF(0.0, 0.0),
            },
        };
        break;
    }
    case SolTrace::Data::SINGLE_AXIS_CURVATURE_SECTION: break;
    case SolTrace::Data::IRREGULAR_TRIANGLE: {
        auto const* ptr = dynamic_cast<SD::IrregularTriangle const*>(&aperture);
        if (!ptr) break;
        ret = {
            {
                .name     = "Point 1",
                .is_point = true,
                .content  = QPointF(ptr->x1, ptr->y1),
            },
            {
                .name     = "Point 2",
                .is_point = true,
                .content  = QPointF(ptr->x2, ptr->y2),
            },
            {
                .name     = "Point 3",
                .is_point = true,
                .content  = QPointF(ptr->x3, ptr->y3),
            },
        };
        break;
    }
    case SolTrace::Data::IRREGULAR_QUADRILATERAL: {
        auto const* ptr =
            dynamic_cast<SD::IrregularQuadrilateral const*>(&aperture);
        if (!ptr) break;
        ret = {
            {
                .name     = "Point 1",
                .is_point = true,
                .content  = QPointF(ptr->x1, ptr->y1),
            },
            {
                .name     = "Point 2",
                .is_point = true,
                .content  = QPointF(ptr->x2, ptr->y2),
            },
            {
                .name     = "Point 3",
                .is_point = true,
                .content  = QPointF(ptr->x3, ptr->y3),
            },
            {
                .name     = "Point 4",
                .is_point = true,
                .content  = QPointF(ptr->x4, ptr->y4),
            },
        };
        break;
    }
    case SolTrace::Data::APERTURE_UNKNOWN: break;
    }

    this->reset(ret);
}

void ApertureParameterModel::write_back(SD::Aperture& aperture) const {
    switch (aperture.my_type) {
    case SolTrace::Data::ANNULUS: {
        auto* ptr = dynamic_cast<SD::Annulus*>(&aperture);
        if (!ptr || m_records.size() < 3) break;
        ptr->inner_radius = scalar_value(m_records[0]);
        ptr->outer_radius = scalar_value(m_records[1]);
        ptr->arc_angle    = scalar_value(m_records[2]);
        break;
    }
    case SolTrace::Data::CIRCLE: {
        auto* ptr = dynamic_cast<SD::Circle*>(&aperture);
        if (!ptr || m_records.isEmpty()) break;
        ptr->diameter = scalar_value(m_records[0]);
        break;
    }
    case SolTrace::Data::HEXAGON: {
        auto* ptr = dynamic_cast<SD::Hexagon*>(&aperture);
        if (!ptr || m_records.isEmpty()) break;
        ptr->circumscribe_diameter = scalar_value(m_records[0]);
        break;
    }
    case SolTrace::Data::RECTANGLE: {
        auto* ptr = dynamic_cast<SD::Rectangle*>(&aperture);
        if (!ptr || m_records.size() < 2) break;
        auto corner = point_value(m_records[0]);
        auto size   = point_value(m_records[1]);
        ptr->set_x_coord(corner.x());
        ptr->set_y_coord(corner.y());
        ptr->set_x_length(size.x());
        ptr->set_y_length(size.y());
        break;
    }
    case SolTrace::Data::EQUILATERAL_TRIANGLE: {
        auto* ptr = dynamic_cast<SD::EqualateralTriangle*>(&aperture);
        if (!ptr || m_records.isEmpty()) break;
        ptr->circumscribe_diameter = scalar_value(m_records[0]);
        break;
    }
    case SolTrace::Data::SINGLE_AXIS_CURVATURE_SECTION:
        break;
    case SolTrace::Data::IRREGULAR_TRIANGLE: {
        auto* ptr = dynamic_cast<SD::IrregularTriangle*>(&aperture);
        if (!ptr || m_records.size() < 3) break;
        auto p1 = point_value(m_records[0]);
        auto p2 = point_value(m_records[1]);
        auto p3 = point_value(m_records[2]);
        ptr->x1 = p1.x();
        ptr->y1 = p1.y();
        ptr->x2 = p2.x();
        ptr->y2 = p2.y();
        ptr->x3 = p3.x();
        ptr->y3 = p3.y();
        break;
    }
    case SolTrace::Data::IRREGULAR_QUADRILATERAL: {
        auto* ptr = dynamic_cast<SD::IrregularQuadrilateral*>(&aperture);
        if (!ptr || m_records.size() < 4) break;
        auto p1 = point_value(m_records[0]);
        auto p2 = point_value(m_records[1]);
        auto p3 = point_value(m_records[2]);
        auto p4 = point_value(m_records[3]);
        ptr->x1 = p1.x();
        ptr->y1 = p1.y();
        ptr->x2 = p2.x();
        ptr->y2 = p2.y();
        ptr->x3 = p3.x();
        ptr->y3 = p3.y();
        ptr->x4 = p4.x();
        ptr->y4 = p4.y();
        break;
    }
    case SolTrace::Data::APERTURE_UNKNOWN:
        break;
    }
}

} // namespace db
