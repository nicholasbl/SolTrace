#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <vector>

#include <glm/vec3.hpp>

#include "aperture.hpp"
#include "surface.hpp"

namespace SD = SolTrace::Data;


namespace db {

struct Vertex {
    glm::vec3    position;
    glm::vec3    normal;
    glm::vec2    uv; // quick geometry only supports floating
};

struct Mesh {
    std::vector<Vertex>   vertex;
    std::vector<uint32_t> index;
};

struct SurfaceGenerationOptions {
    std::array<uint32_t, 2> height_field_resolution       = { 24, 24 };
    uint32_t                radial_subdivisions           = 24;
    uint32_t                perimeter_subdivisions        = 64;
    uint32_t                cylinder_angular_subdivisions = 64;
    uint32_t                cylinder_length_subdivisions  = 24;
};

std::optional<Mesh>
generate_surface(SD::surface_ptr const&          surface,
                 SD::aperture_ptr const&         aperture,
                 SurfaceGenerationOptions const& options = {});

} // namespace db
