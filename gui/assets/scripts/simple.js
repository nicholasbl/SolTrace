/*
TITLE Flat Mirror Arc With Cylindrical Absorber
DESC Creates a vertical cylindrical absorber and flat mirror facets arranged on a 180 degree ground arc.
DESC The mirror planes stand on the ground and face inward toward the absorber.
PROPERTY mirror_count integer 9 1..=200
PROPERTY arc_radius real 35 1..1000
PROPERTY mirror_width real 6 0.01..
PROPERTY mirror_height real 4 0.01..
PROPERTY absorber_height real 12 0.01..
PROPERTY absorber_radius real 2 0.01..
*/
(function(
    mirror_count,
    arc_radius,
    mirror_width,
    mirror_height,
    absorber_height,
    absorber_radius
) {
    function degrees_to_radians(degrees) {
        return degrees * Math.PI / 180.0
    }

    function multiply_quaternion(a, b) {
        return [
            a[0] * b[0] - a[1] * b[1] - a[2] * b[2] - a[3] * b[3],
            a[0] * b[1] + a[1] * b[0] + a[2] * b[3] - a[3] * b[2],
            a[0] * b[2] - a[1] * b[3] + a[2] * b[0] + a[3] * b[1],
            a[0] * b[3] + a[1] * b[2] - a[2] * b[1] + a[3] * b[0],
        ]
    }

    function z_quaternion(angle) {
        const half = angle / 2.0
        return [Math.cos(half), 0.0, 0.0, Math.sin(half)]
    }

    function x_quaternion(angle) {
        const half = angle / 2.0
        return [Math.cos(half), Math.sin(half), 0.0, 0.0]
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
            aperture_type: "RECTANGLE",
            x_length: absorber_radius * 2.0,
            y_length: absorber_height,
            x_coord: -absorber_radius,
            y_coord: 0.0,
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
            y_coord: 0.0,
        },
        surface: {
            surface_type: "FLAT",
        },
    })

    const absorber = db.create()
    db.set_identity(absorber, "Cylindrical absorber")
    db.set_transform(absorber, {
        position: [0.0, absorber_radius, 0.0],
        rotation: x_quaternion(degrees_to_radians(90.0)),
    })
    db.set_material_of(absorber, absorber_material)
    db.set_geometry_of(absorber, absorber_geometry)

    const mirrors = []
    const denominator = Math.max(1, mirror_count - 1)

    for (let i = 0; i < mirror_count; ++i) {
        const fraction = i / denominator
        const angle = degrees_to_radians(-90.0 + 180.0 * fraction)
        const x = arc_radius * Math.sin(angle)
        const y = -arc_radius * Math.cos(angle)
        const rotation = multiply_quaternion(
            z_quaternion(angle + Math.PI),
            x_quaternion(degrees_to_radians(90.0))
        )

        const mirror = db.create()
        db.set_identity(mirror, "Mirror " + String(i + 1))
        db.set_transform(mirror, {
            position: [x, y, 0.0],
            rotation: rotation,
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
