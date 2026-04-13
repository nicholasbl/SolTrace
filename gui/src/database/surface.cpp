#include "surface.h"

#define GLM_ENABLE_EXPERIMENTAL 1

#include <glm/ext/scalar_constants.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/component_wise.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

#include <QDebug>

namespace db {

namespace {

constexpr double PI = glm::pi<double>();

struct Mesh2D {
    std::vector<glm::dvec2> vertex;
    std::vector<glm::uvec3> triangles;

    template <class F>
    Mesh map(F&& f) const {
        Mesh ret;

        ret.triangles = triangles;

        ret.vertex.reserve(vertex.size());

        for (auto p : vertex) {
            ret.vertex.push_back(f(p));
        }

        return ret;
    }

    std::tuple<glm::dvec2, glm::dvec2> bb_2d() const {
        if (vertex.empty()) { return { glm::dvec2 { 0 }, glm::dvec2 { 0 } }; }
        glm::dvec2 mins = vertex[0];
        glm::dvec2 maxs = vertex[0];

        for (auto const& p : vertex) {
            mins = glm::min(p, mins);
            maxs = glm::max(p, maxs);
        }

        return { mins, maxs };
    }
};

double saturate(double value) {
    return std::clamp(value, 0.0, 1.0);
}

glm::dvec2 saturate(glm::dvec2 value) {
    return glm::clamp(value, 0.0, 1.0);
}

Vertex make_vertex(double    x,
                   double    y,
                   double    z,
                   glm::vec3 normal,
                   double    u,
                   double    v) {
    return Vertex {
        .position = glm::vec3(x, y, z),
        .normal   = normal,
        .uv       = saturate({ u, v }),
    };
}

// TODO: check all UVs and see if we can do better

std::optional<Mesh> make_flat(SD::Flat& flat, Mesh2D const& m2d) {
    auto [bb_min, bb_max] = m2d.bb_2d();

    // simple UV

    return m2d.map([&](glm::dvec2 p) {
        return Vertex {
            .position = glm::vec3(p, 0.0),
            .normal   = { 0.0f, 0.0f, 1.0f },
            .uv       = (p - bb_min) / (bb_max - bb_min),
        };
    });
}

std::optional<Mesh> make_cone(SD::Cone& item, Mesh2D const& m2d) {
    if (item.half_angle <= 0.0) {
        qDebug() << Q_FUNC_INFO << "Half angle is <= 0";
        return {};
    }

    auto [bb_min, bb_max] = m2d.bb_2d();

    auto tan_angle = std::tan(item.half_angle);

    return m2d.map([&](glm::dvec2 p) {
        auto z = glm::length(p) / tan_angle;

        glm::vec3 normal = { 0.0f, 0.0f, 1.0f };

        double rho = glm::length(p);
        if (rho > 1e-12) {
            auto scale = 1.0 / (rho * tan_angle);
            normal     = glm::normalize(glm::vec3(-p * scale, 1.0));
        }


        return Vertex {
            .position = glm::vec3(p, z),
            .normal   = normal,
            .uv       = (p - bb_min) / (bb_max - bb_min),
        };
    });
}

std::optional<Mesh> make_para(SD::Parabola& item, Mesh2D const& m2d) {
    if (item.focal_length_x == 0.0 || item.focal_length_y == 0.0) {
        qDebug() << Q_FUNC_INFO << "Focal lengths are zero";
        return {};
    }

    auto [bb_min, bb_max] = m2d.bb_2d();

    auto flen = glm::dvec2(item.focal_length_x, item.focal_length_y);

    return m2d.map([&](glm::dvec2 p) {
        auto z = glm::compAdd(0.25 * (p * p) / flen);

        auto normal = glm::normalize(glm::vec3(-0.5 * p / flen, 1.0));

        return Vertex {
            .position = glm::vec3(p, z),
            .normal   = normal,
            .uv       = (p - bb_min) / (bb_max - bb_min),
        };
    });
}

std::optional<Mesh> make_sphere(SD::Sphere& item, Mesh2D const& m2d) {

    auto [bb_min, bb_max] = m2d.bb_2d();

    return m2d.map([&](glm::dvec2 p) {
        auto p_dot_p = glm::dot(p, p);

        double term = 1.0 - item.vertex_curv * item.vertex_curv * p_dot_p;
        if (term < 0.0) { term = 0.0; }

        auto z = item.vertex_curv * p_dot_p / (1.0 + std::sqrt(term));

        glm::vec3 normal;

        if (item.vertex_curv == 0.0) {
            normal = { 0.0f, 0.0f, 1.0f };
        } else {
            double radius = 1.0 / item.vertex_curv;
            normal        = glm::normalize(glm::vec3(-p, radius - z));
        }

        return Vertex {
            .position = glm::vec3(p, z),
            .normal   = normal,
            .uv       = (p - bb_min) / (bb_max - bb_min),
        };
    });
}

// Don't do UV's in the base mesh generation as that does not
// consider final distortion

Mesh2D generate_rectangle_aperture(SD::Rectangle const&            rect,
                                   SurfaceGenerationOptions const& options) {
    Mesh2D mesh;

    uint32_t x_steps = std::max<uint32_t>(1, options.height_field_resolution.x);
    uint32_t y_steps = std::max<uint32_t>(1, options.height_field_resolution.y);

    double x0 = rect.x_coord();
    double y0 = rect.y_coord();
    double x1 = x0 + rect.x_length();
    double y1 = y0 + rect.y_length();

    mesh.vertex.reserve((x_steps + 1) * (y_steps + 1));
    mesh.triangles.reserve(x_steps * y_steps * 2);

    for (uint32_t iy = 0; iy <= y_steps; ++iy) {
        double v = static_cast<double>(iy) / y_steps;
        double y = y0 + (y1 - y0) * v;
        for (uint32_t ix = 0; ix <= x_steps; ++ix) {
            double u = static_cast<double>(ix) / x_steps;
            double x = x0 + (x1 - x0) * u;
            mesh.vertex.push_back({ x, y });
        }
    }

    uint32_t stride = x_steps + 1;
    for (uint32_t iy = 0; iy < y_steps; ++iy) {
        for (uint32_t ix = 0; ix < x_steps; ++ix) {
            uint32_t a = iy * stride + ix;
            uint32_t b = a + 1;
            uint32_t c = (iy + 1) * stride + ix;
            uint32_t d = c + 1;

            mesh.triangles.push_back({ a, c, b });
            mesh.triangles.push_back({ b, c, d });
        }
    }

    return mesh;
}

Mesh2D generate_circle_aperture(double                          radius,
                                SurfaceGenerationOptions const& options) {
    Mesh2D mesh;
    if (radius <= 0.0) {
        qDebug() << Q_FUNC_INFO << "Circle aperture radius is zero";
        return mesh;
    }

    uint32_t radial_steps = std::max<uint32_t>(1, options.radial_subdivisions);
    uint32_t angle_steps =
        std::max<uint32_t>(3, options.perimeter_subdivisions);

    mesh.vertex.reserve(1 + radial_steps * angle_steps);
    mesh.triangles.reserve(angle_steps * 3 +
                           (radial_steps - 1) * angle_steps * 2);

    mesh.vertex.push_back({ 0.0, 0.0 });

    for (uint32_t ir = 1; ir <= radial_steps; ++ir) {
        double r = radius * static_cast<double>(ir) / radial_steps;
        for (uint32_t ia = 0; ia < angle_steps; ++ia) {
            double theta = (2.0 * PI * ia) / angle_steps;
            mesh.vertex.push_back({ r * std::cos(theta), r * std::sin(theta) });
        }
    }

    for (uint32_t ia = 0; ia < angle_steps; ++ia) {
        uint32_t a = 1 + ia;
        uint32_t b = 1 + ((ia + 1) % angle_steps);
        mesh.triangles.push_back({ 0, a, b });
    }

    for (uint32_t ir = 1; ir < radial_steps; ++ir) {
        uint32_t inner = 1 + (ir - 1) * angle_steps;
        uint32_t outer = 1 + ir * angle_steps;
        for (uint32_t ia = 0; ia < angle_steps; ++ia) {
            uint32_t next = (ia + 1) % angle_steps;
            uint32_t a    = inner + ia;
            uint32_t b    = inner + next;
            uint32_t c    = outer + ia;
            uint32_t d    = outer + next;

            mesh.triangles.push_back({ a, c, b });
            mesh.triangles.push_back({ b, c, d });
        }
    }

    return mesh;
}

Mesh2D generate_annulus_aperture(SD::Annulus const&              annulus,
                                 SurfaceGenerationOptions const& options) {
    Mesh2D mesh;

    if (annulus.outer_radius <= 0.0 || annulus.inner_radius < 0.0 ||
        annulus.inner_radius >= annulus.outer_radius ||
        annulus.arc_angle <= 0.0) {
        qDebug() << Q_FUNC_INFO << "Annulus radii are nonsensical";
        return mesh;
    }

    uint32_t radial_steps = std::max<uint32_t>(1, options.radial_subdivisions);
    uint32_t angle_steps =
        std::max<uint32_t>(3, options.perimeter_subdivisions);

    double sweep       = annulus.arc_angle;
    double start_angle = -0.5 * sweep;
    bool   closed      = std::abs(sweep - 2.0 * PI) < 1e-6;

    uint32_t columns = closed ? angle_steps : (angle_steps + 1);

    mesh.vertex.reserve((radial_steps + 1) * columns);
    mesh.triangles.reserve(radial_steps * angle_steps * 2);

    for (uint32_t ir = 0; ir <= radial_steps; ++ir) {
        double t = static_cast<double>(ir) / radial_steps;
        double r = annulus.inner_radius +
                   (annulus.outer_radius - annulus.inner_radius) * t;

        for (uint32_t ia = 0; ia < columns; ++ia) {
            double u     = static_cast<double>(ia) / angle_steps;
            double theta = start_angle + sweep * u;
            mesh.vertex.push_back({ r * std::cos(theta), r * std::sin(theta) });
        }
    }

    uint32_t stride = columns;
    for (uint32_t ir = 0; ir < radial_steps; ++ir) {
        for (uint32_t ia = 0; ia < angle_steps; ++ia) {
            uint32_t next = closed ? ((ia + 1) % angle_steps) : (ia + 1);
            uint32_t a    = ir * stride + ia;
            uint32_t b    = ir * stride + next;
            uint32_t c    = (ir + 1) * stride + ia;
            uint32_t d    = (ir + 1) * stride + next;

            mesh.triangles.push_back({ a, c, b });
            mesh.triangles.push_back({ b, c, d });
        }
    }

    return mesh;
}

Mesh2D generate_polygon_aperture(std::vector<glm::dvec2> const&  corners,
                                 SurfaceGenerationOptions const& options) {
    Mesh2D mesh;
    if (corners.size() < 3) {
        qDebug() << Q_FUNC_INFO << "Polygon aperture has <3 vertex";
        return mesh;
    }

    uint32_t radial_steps = std::max<uint32_t>(1, options.radial_subdivisions);
    uint32_t ring_size    = static_cast<uint32_t>(corners.size());

    glm::dvec2 center(0.0);
    for (auto const& corner : corners)
        center += corner;
    center /= static_cast<double>(corners.size());

    mesh.vertex.reserve(1 + radial_steps * ring_size);
    mesh.triangles.reserve(ring_size * 3 + (radial_steps - 1) * ring_size * 2);

    mesh.vertex.push_back(center);

    for (uint32_t ir = 1; ir <= radial_steps; ++ir) {
        double t = static_cast<double>(ir) / radial_steps;
        for (auto const& corner : corners) {
            mesh.vertex.push_back(glm::mix(center, corner, t));
        }
    }

    uint32_t first_ring = 1;
    for (uint32_t i = 0; i < ring_size; ++i) {
        uint32_t a = first_ring + i;
        uint32_t b = first_ring + ((i + 1) % ring_size);
        mesh.triangles.push_back({ 0, a, b });
    }

    for (uint32_t ir = 1; ir < radial_steps; ++ir) {
        uint32_t inner = 1 + (ir - 1) * ring_size;
        uint32_t outer = 1 + ir * ring_size;
        for (uint32_t i = 0; i < ring_size; ++i) {
            uint32_t next = (i + 1) % ring_size;
            uint32_t a    = inner + i;
            uint32_t b    = inner + next;
            uint32_t c    = outer + i;
            uint32_t d    = outer + next;

            mesh.triangles.push_back({ a, c, b });
            mesh.triangles.push_back({ b, c, d });
        }
    }

    return mesh;
}

std::optional<Mesh2D>
generate_aperture_mesh(SD::aperture_ptr const&         aperture,
                       SurfaceGenerationOptions const& options) {
    if (!aperture) {
        qDebug() << Q_FUNC_INFO << "Missing aperture";
        return std::nullopt;
    }

    switch (aperture->my_type) {
    case SD::RECTANGLE: {
        auto rect = std::dynamic_pointer_cast<SD::Rectangle const>(aperture);
        if (!rect || rect->x_length() <= 0.0 || rect->y_length() <= 0.0) {
            qDebug() << Q_FUNC_INFO << "Bad rectangle";
            return std::nullopt;
        }
        return generate_rectangle_aperture(*rect, options);
    }
    case SD::CIRCLE: {
        auto circle = std::dynamic_pointer_cast<SD::Circle const>(aperture);
        if (!circle || circle->diameter <= 0.0) {
            qDebug() << Q_FUNC_INFO << "Bad circle";
            return std::nullopt;
        }
        return generate_circle_aperture(circle->diameter * 0.5, options);
    }
    case SD::ANNULUS: {
        auto annulus = std::dynamic_pointer_cast<SD::Annulus const>(aperture);
        if (!annulus) {
            qDebug() << Q_FUNC_INFO << "Bad annulus";
            return std::nullopt;
        }
        return generate_annulus_aperture(*annulus, options);
    }
    case SD::EQUILATERAL_TRIANGLE: {
        auto tri =
            std::dynamic_pointer_cast<SD::EqualateralTriangle const>(aperture);
        if (!tri || tri->circumscribe_diameter <= 0.0) {
            qDebug() << Q_FUNC_INFO << "Bad eq triangle";
            return std::nullopt;
        }

        double r = tri->circumscribe_diameter * 0.5;
        return generate_polygon_aperture(
            { { 0.0, r },
              { r * std::cos(-PI / 6.0), r * std::sin(-PI / 6.0) },
              { r * std::cos(7.0 * PI / 6.0), r * std::sin(7.0 * PI / 6.0) } },
            options);
    }
    case SD::HEXAGON: {
        auto hex = std::dynamic_pointer_cast<SD::Hexagon const>(aperture);
        if (!hex || hex->circumscribe_diameter <= 0.0) {
            qDebug() << Q_FUNC_INFO << "Bad hexagon";
            return std::nullopt;
        }

        double                  r = hex->circumscribe_diameter * 0.5;
        std::vector<glm::dvec2> corners;
        corners.reserve(6);
        for (int i = 0; i < 6; ++i) {
            double theta = i * PI / 3.0;
            corners.push_back({ r * std::cos(theta), r * std::sin(theta) });
        }
        return generate_polygon_aperture(corners, options);
    }
    case SD::IRREGULAR_TRIANGLE: {
        auto tri =
            std::dynamic_pointer_cast<SD::IrregularTriangle const>(aperture);
        if (!tri) {
            qDebug() << Q_FUNC_INFO << "Bad irregular triangle";
            return std::nullopt;
        }
        return generate_polygon_aperture({ { tri->x1, tri->y1 },
                                           { tri->x2, tri->y2 },
                                           { tri->x3, tri->y3 } },
                                         options);
    }
    case SD::IRREGULAR_QUADRILATERAL: {
        auto quad = std::dynamic_pointer_cast<SD::IrregularQuadrilateral const>(
            aperture);
        if (!quad) {
            qDebug() << Q_FUNC_INFO << "Bad irregular quad";
            return std::nullopt;
        }
        return generate_polygon_aperture({ { quad->x1, quad->y1 },
                                           { quad->x2, quad->y2 },
                                           { quad->x3, quad->y3 },
                                           { quad->x4, quad->y4 } },
                                         options);
    }
    case SD::SINGLE_AXIS_CURVATURE_SECTION:
    case SD::APERTURE_UNKNOWN:
        qDebug() << Q_FUNC_INFO << "Aperture not supported";
        return std::nullopt;
    }

    qDebug() << Q_FUNC_INFO << "Unknown aperture";
    return std::nullopt;
}

std::optional<Mesh>
generate_height_field_surface(SD::surface_ptr const&          surface,
                              SD::aperture_ptr const&         aperture,
                              SurfaceGenerationOptions const& options) {
    if (!surface || !aperture) {
        qDebug() << Q_FUNC_INFO << "Missing surface or aperture";
        return std::nullopt;
    }

    auto aperture_mesh = generate_aperture_mesh(aperture, options);
    if (!aperture_mesh || aperture_mesh->vertex.empty() ||
        aperture_mesh->triangles.empty()) {

        qDebug() << Q_FUNC_INFO << "Unable to build aperture mesh";
        return std::nullopt;
    }

    double min_x = std::numeric_limits<double>::max();
    double min_y = std::numeric_limits<double>::max();
    double max_x = -std::numeric_limits<double>::max();
    double max_y = -std::numeric_limits<double>::max();

    for (auto const& point : aperture_mesh->vertex) {
        min_x = std::min(min_x, point.x);
        min_y = std::min(min_y, point.y);
        max_x = std::max(max_x, point.x);
        max_y = std::max(max_y, point.y);
    }

    std::optional<Mesh> ret;

    switch (surface->my_type) {
    case SD::CONE:
        if (auto ptr = std::dynamic_pointer_cast<SD::Cone>(surface); ptr) {
            ret = make_cone(*ptr, *aperture_mesh);
        }
        break;
    case SD::FLAT:
        if (auto ptr = std::dynamic_pointer_cast<SD::Flat>(surface); ptr) {
            ret = make_flat(*ptr, *aperture_mesh);
        }
        break;
    case SD::PARABOLA:
        if (auto ptr = std::dynamic_pointer_cast<SD::Parabola>(surface); ptr) {
            ret = make_para(*ptr, *aperture_mesh);
        }
        break;
    case SD::SPHERE:
        if (auto ptr = std::dynamic_pointer_cast<SD::Sphere>(surface); ptr) {
            ret = make_sphere(*ptr, *aperture_mesh);
        }
        break;

    default: break;
    }

    return ret;
}

std::optional<Mesh>
generate_cylinder_surface(SD::surface_ptr const&          surface,
                          SD::aperture_ptr const&         aperture,
                          SurfaceGenerationOptions const& options) {
    auto cylinder = std::dynamic_pointer_cast<SD::Cylinder const>(surface);
    auto rect     = std::dynamic_pointer_cast<SD::Rectangle const>(aperture);

    if (!cylinder || !rect || cylinder->radius <= 0.0) {
        qDebug() << Q_FUNC_INFO << "Unable to build cylinder surface";
        return std::nullopt;
    }

    Mesh mesh;

    uint32_t angular_steps =
        std::max<uint32_t>(3, options.cylinder_angular_subdivisions);
    uint32_t length_steps =
        std::max<uint32_t>(1, options.cylinder_length_subdivisions);

    double radius = cylinder->radius;
    double y0     = rect->y_coord();
    double y1     = rect->y_coord() + rect->y_length();

    mesh.vertex.reserve((angular_steps + 1) * (length_steps + 1));
    mesh.triangles.reserve(angular_steps * length_steps * 2);

    for (uint32_t iy = 0; iy <= length_steps; ++iy) {
        double v = static_cast<double>(iy) / length_steps;
        double y = y0 + (y1 - y0) * v;

        for (uint32_t ia = 0; ia <= angular_steps; ++ia) {
            double u      = static_cast<double>(ia) / angular_steps;
            double theta  = u * 2.0 * PI;
            double x      = radius * std::cos(theta);
            double z      = radius + radius * std::sin(theta);
            auto   normal = glm::normalize(glm::vec3(x, 0.0, z - radius));

            mesh.vertex.push_back(make_vertex(x, y, z, normal, u, v));
        }
    }

    uint32_t stride = angular_steps + 1;
    for (uint32_t iy = 0; iy < length_steps; ++iy) {
        for (uint32_t ia = 0; ia < angular_steps; ++ia) {
            uint32_t a = iy * stride + ia;
            uint32_t b = a + 1;
            uint32_t c = (iy + 1) * stride + ia;
            uint32_t d = c + 1;

            mesh.triangles.push_back({ a, c, b });
            mesh.triangles.push_back({ b, c, d });
        }
    }

    return mesh;
}

} // namespace

std::optional<Mesh> generate_surface(SD::surface_ptr const&          surface,
                                     SD::aperture_ptr const&         aperture,
                                     SurfaceGenerationOptions const& options) {
    if (!surface || !aperture) return std::nullopt;

    switch (surface->my_type) {
    case SD::CYLINDER:
        return generate_cylinder_surface(surface, aperture, options);
    case SD::FLAT:
    case SD::CONE:
    case SD::PARABOLA:
    case SD::SPHERE:
        return generate_height_field_surface(surface, aperture, options);
    case SD::TORUS:
    case SD::HYPER:
    case SD::GENERAL_SPENCER_MURTY:
    case SD::SURFACE_UNKNOWN:
        qDebug() << Q_FUNC_INFO << "Unsupported surface type";
        return std::nullopt;
    }

    qDebug() << Q_FUNC_INFO << "Unknown surface type";
    return std::nullopt;
}

} // namespace db
