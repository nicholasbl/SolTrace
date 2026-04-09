#include "surfaceparametermodel.h"

#include <cmath>

namespace db {

namespace {

QVector<SurfaceParameter> make_parameters_for(SD::SurfaceType type) {
    QVector<SurfaceParameter> ret;

    switch (type) {
    case SD::CONE:
        ret = {
            {
                .name        = "Half angle",
                .content     = M_PI / 4.0,
                .min_bounded = true,
                .max_bounded = true,
                .min         = 0.0,
                .max         = M_PI / 2.0,
            },
        };
        break;
    case SD::CYLINDER:
        ret = {
            {
                .name        = "Vertex curvature",
                .content     = 1.0,
                .min_bounded = true,
                .min         = 0.0,
            },
        };
        break;
    case SD::FLAT: break;
    case SD::PARABOLA:
        ret = {
            { .name = "Vertex curvature X", .content = 1.0 },
            { .name = "Vertex curvature Y", .content = 1.0 },
        };
        break;
    case SD::SPHERE:
        ret = {
            { .name = "Vertex curvature", .content = 1.0 },
        };
        break;
    case SD::HYPER:
    case SD::GENERAL_SPENCER_MURTY:
    case SD::TORUS:
    case SD::SURFACE_UNKNOWN: break;
    }

    return ret;
}

double curvature_from_focal_length(double focal_length) {
    if (focal_length == 0.0) return 0.0;
    return 1.0 / (2.0 * focal_length);
}

double focal_length_from_curvature(double curvature) {
    if (curvature == 0.0) return 0.0;
    return 1.0 / (2.0 * curvature);
}

} // namespace

SurfaceParameterModel::SurfaceParameterModel(QObject* parent)
    : StructTableModel(parent) {
    connect(this,
            &SurfaceParameterModel::dataChanged,
            this,
            &SurfaceParameterModel::updated);
}

void SurfaceParameterModel::set_for(SD::SurfaceType type) {
    this->reset(make_parameters_for(type));
}

void SurfaceParameterModel::set_from(SD::Surface const& surface) {
    auto ret = make_parameters_for(surface.my_type);

    switch (surface.my_type) {
    case SD::CONE: {
        auto const* ptr = dynamic_cast<SD::Cone const*>(&surface);
        if (!ptr) break;
        ret = {
            {
                .name        = "Half angle",
                .content     = ptr->half_angle,
                .min_bounded = true,
                .max_bounded = true,
                .min         = 0.0,
                .max         = M_PI / 2.0,
            },
        };
        break;
    }
    case SD::CYLINDER: {
        auto const* ptr = dynamic_cast<SD::Cylinder const*>(&surface);
        if (!ptr) break;
        ret = {
            {
                .name        = "Vertex curvature",
                .content     = ptr->radius == 0.0 ? 0.0 : 1.0 / ptr->radius,
                .min_bounded = true,
                .min         = 0.0,
            },
        };
        break;
    }
    case SD::FLAT: break;
    case SD::PARABOLA: {
        auto const* ptr = dynamic_cast<SD::Parabola const*>(&surface);
        if (!ptr) break;
        ret = {
            {
                .name    = "Vertex curvature X",
                .content = curvature_from_focal_length(ptr->focal_length_x),
            },
            {
                .name    = "Vertex curvature Y",
                .content = curvature_from_focal_length(ptr->focal_length_y),
            },
        };
        break;
    }
    case SD::SPHERE: {
        auto const* ptr = dynamic_cast<SD::Sphere const*>(&surface);
        if (!ptr) break;
        ret = {
            { .name = "Vertex curvature", .content = ptr->vertex_curv },
        };
        break;
    }
    case SD::HYPER:
    case SD::GENERAL_SPENCER_MURTY:
    case SD::TORUS:
    case SD::SURFACE_UNKNOWN: break;
    }

    this->reset(ret);
}

void SurfaceParameterModel::write_back(SD::Surface& surface) const {
    switch (surface.my_type) {
    case SD::CONE: {
        auto* ptr = dynamic_cast<SD::Cone*>(&surface);
        if (!ptr || m_records.isEmpty()) break;
        ptr->half_angle = m_records[0].content;
        break;
    }
    case SD::CYLINDER: {
        auto* ptr = dynamic_cast<SD::Cylinder*>(&surface);
        if (!ptr || m_records.isEmpty()) break;
        ptr->radius =
            m_records[0].content == 0.0 ? 0.0 : 1.0 / m_records[0].content;
        break;
    }
    case SD::FLAT: break;
    case SD::PARABOLA: {
        auto* ptr = dynamic_cast<SD::Parabola*>(&surface);
        if (!ptr || m_records.size() < 2) break;
        ptr->focal_length_x = focal_length_from_curvature(m_records[0].content);
        ptr->focal_length_y = focal_length_from_curvature(m_records[1].content);
        break;
    }
    case SD::SPHERE: {
        auto* ptr = dynamic_cast<SD::Sphere*>(&surface);
        if (!ptr || m_records.isEmpty()) break;
        ptr->vertex_curv = m_records[0].content;
        break;
    }
    case SD::HYPER:
    case SD::GENERAL_SPENCER_MURTY:
    case SD::TORUS:
    case SD::SURFACE_UNKNOWN: break;
    }
}

QVector<double> SurfaceParameterModel::arguments() const {
    QVector<double> ret;
    ret.reserve(m_records.size());

    for (auto const& record : m_records) {
        ret.push_back(record.content);
    }

    return ret;
}

} // namespace db
