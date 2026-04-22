/*
TITLE Flat Mirror Arc With Cylindrical Absorber
DESC Creates a small cylindrical absorber and a configurable count of flat mirror facets arranged on a 180 degree arc.
DESC This is intentionally minimal and uses the script database interface shape directly.
PROPERTY mirror_count integer 1..=200
PROPERTY arc_radius real 1..1000
PROPERTY mirror_width real 0.01..
PROPERTY mirror_height real 0.01..
PROPERTY absorber_diameter real 0.01..
PROPERTY absorber_radius real 0.01..
*/
(function(
    mirror_count,
    arc_radius,
    mirror_width,
    mirror_height,
    absorber_diameter,
    absorber_radius
) {
    function degrees_to_radians(degrees) {
        return degrees * Math.PI / 180.0
    }

    function yaw_quaternion(yaw) {
        const half = yaw / 2.0
        return [Math.cos(half), 0.0, Math.sin(half), 0.0]
    }

    const absorber_material = db.create_material()
    db.set_identity(absorber_material, "Absorber material")
    db.set_material_properties(absorber_material, {
        front: {
            my_type: "REFLECTION",
            error_distribution_type: "GAUSSIAN",
            transmissivity: 0.0,
            reflectivity: 0.0,
            slope_error: 0.0,
            specularity_error: 0.0,
            refraction_index_front: 1.0,
            refraction_index_back: 1.0,
        },
        back: {
            my_type: "REFLECTION",
            error_distribution_type: "GAUSSIAN",
            transmissivity: 0.0,
            reflectivity: 0.0,
            slope_error: 0.0,
            specularity_error: 0.0,
            refraction_index_front: 1.0,
            refraction_index_back: 1.0,
        },
    })

    const mirror_material = db.create_material()
    db.set_identity(mirror_material, "Ideal mirror material")
    db.set_material_properties(mirror_material, {
        front: {
            my_type: "REFLECTION",
            error_distribution_type: "GAUSSIAN",
            transmissivity: 0.0,
            reflectivity: 1.0,
            slope_error: 0.0,
            specularity_error: 0.0,
            refraction_index_front: 1.0,
            refraction_index_back: 1.0,
        },
        back: {
            my_type: "REFLECTION",
            error_distribution_type: "GAUSSIAN",
            transmissivity: 0.0,
            reflectivity: 0.0,
            slope_error: 0.0,
            specularity_error: 0.0,
            refraction_index_front: 1.0,
            refraction_index_back: 1.0,
        },
    })

    const absorber_geometry = db.create_geometry()
    db.set_identity(absorber_geometry, "Cylindrical absorber geometry")
    db.set_geometry_properties(absorber_geometry, {
        aperture: {
            aperture_type: "CIRCLE",
            diameter: absorber_diameter,
        },
        surface: {
            surface_type: "CYLINDER",
            radius: absorber_radius,
        },
    })

    const mirror_geometry = db.create_geometry()
    db.set_identity(mirror_geometry, "Flat mirror geometry")
    db.set_geometry_properties(mirror_geometry, {
        aperture: {
            aperture_type: "RECTANGLE",
            x_length: mirror_width,
            y_length: mirror_height,
            x_coord: -mirror_width / 2.0,
            y_coord: -mirror_height / 2.0,
        },
        surface: {
            surface_type: "FLAT",
        },
    })

    const absorber = db.create()
    db.set_identity(absorber, "Cylindrical absorber")
    db.set_transform(absorber, {
        position: [0.0, 0.0, 0.0],
        rotation: [1.0, 0.0, 0.0, 0.0],
    })
    db.set_material_of(absorber, absorber_material)
    db.set_geometry_of(absorber, absorber_geometry)

    const mirrors = []
    const denominator = Math.max(1, mirror_count - 1)

    for (let i = 0; i < mirror_count; ++i) {
        const fraction = i / denominator
        const angle = degrees_to_radians(-90.0 + 180.0 * fraction)
        const x = arc_radius * Math.sin(angle)
        const z = -arc_radius * Math.cos(angle)

        const mirror = db.create()
        db.set_identity(mirror, "Mirror " + String(i + 1))
        db.set_transform(mirror, {
            position: [x, 0.0, z],
            rotation: yaw_quaternion(-angle),
        })
        db.set_material_of(mirror, mirror_material)
        db.set_geometry_of(mirror, mirror_geometry)

        mirrors.push(mirror)
    }

    return {
        absorber: absorber,
        mirrors: mirrors,
        absorber_material: absorber_material,
        mirror_material: mirror_material,
        absorber_geometry: absorber_geometry,
        mirror_geometry: mirror_geometry,
    }
})
