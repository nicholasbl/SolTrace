#include <optix.h>
// #include <cuda/helpers.h>
#include "Soltrace.h"
#include <stdio.h>
#include "GeometryDataST.h"

extern "C"
{
    __constant__ OptixCSP::LaunchParams params;
}

extern "C" __device__ __inline__ float ray_distance_to_plane(float3 ro, float3 rd, float4 plane)
{
    const float3 n = make_float3(plane);
    return (plane.w - dot(n, ro)) / dot(rd, n);
}

extern "C" __global__ void __intersection__parallelogram()
{
    int i = optixGetPrimitiveIndex();
    const OptixCSP::GeometryDataST::Parallelogram &parallelogram = params.geometry_data_array[i].getParallelogram();

    // Get ray information: origin, direction, and min/max distances over which ray should be tested
    const float3 ray_orig = optixGetWorldRayOrigin();
    const float3 ray_dir = optixGetWorldRayDirection();
    const float ray_tmin = optixGetRayTmin(), ray_tmax = optixGetRayTmax();

    // // Compute ray intersection point
    // float3 n  = make_float3( parallelogram.plane );
    // float  dt = dot( ray_dir, n );
    // // Compute distance t (point of intersection) along ray direction from ray origin
    // float  t  = ( parallelogram.plane.w - dot( n, ray_orig ) ) / dt;
    float t = ray_distance_to_plane(ray_orig, ray_dir, parallelogram.plane);
    const float4 n = parallelogram.plane;

    // Verify intersection distance and Report ray intersection point
    if (t > ray_tmin && t < ray_tmax)
    {
        float3 p = ray_orig + ray_dir * t;
        float3 vi = p - parallelogram.anchor;
        float a1 = dot(parallelogram.v1, vi);
        if (a1 >= 0 && a1 <= 1)
        {
            float a2 = dot(parallelogram.v2, vi);
            if (a2 >= 0 && a2 <= 1)
            {
                optixReportIntersection(t,
                                        0,
                                        __float_as_uint(n.x),
                                        __float_as_uint(n.y),
                                        __float_as_uint(n.z));
            }
        }
    }
}

extern "C" __global__ void __intersection__rectangle_flat()
{

    const OptixCSP::GeometryDataST::Rectangle_Flat &rectangle = params.geometry_data_array[optixGetPrimitiveIndex()].getRectangle_Flat();

    const float3 ray_orig = optixGetWorldRayOrigin();
    const float3 ray_dir = optixGetWorldRayDirection();
    const float ray_tmin = optixGetRayTmin();
    const float ray_tmax = optixGetRayTmax();

    // // Get plane normal and distance
    // float3 n = make_float3(rectangle.plane);
    // float dt = dot(ray_dir, n);

    // // Compute distance t (point of intersection) along ray direction from ray origin
    // float t = (rectangle.plane.w - dot(n, ray_orig)) / dt;
    float t = ray_distance_to_plane(ray_orig, ray_dir, rectangle.plane);
    const float4 n = rectangle.plane;

    // Verify intersection distance
    if (t > ray_tmin && t < ray_tmax)
    {
        // Compute intersection point
        float3 p = ray_orig + ray_dir * t;

        // Compute vector from center to intersection point
        float3 v = p - rectangle.center;

        // Project onto x and y to get local coordinates
        float x = dot(rectangle.x, v);
        float y = dot(rectangle.y, v);

        // Check if point is within rectangle bounds
        if (x >= -rectangle.width / 2 && x <= rectangle.width / 2 &&
            y >= -rectangle.height / 2 && y <= rectangle.height / 2)
        {
            optixReportIntersection(t,
                                    0,
                                    __float_as_uint(n.x),
                                    __float_as_uint(n.y),
                                    __float_as_uint(n.z));
        }
    }
}

extern "C" __global__ void __intersection__cylinder_y()
{
    const OptixCSP::GeometryDataST::Cylinder_Y &cyl = params.geometry_data_array[optixGetPrimitiveIndex()].getCylinder_Y();

    // Get ray information: origin, direction, and min/max distances over which ray should be tested
    const float3 ray_orig = optixGetWorldRayOrigin();
    const float3 ray_dir = normalize(optixGetWorldRayDirection());
    const float ray_tmin = optixGetRayTmin();
    const float ray_tmax = optixGetRayTmax();

    // Transform ray to the cylinder's local coordinate system
    float3 local_ray_orig = ray_orig - cyl.center;
    float3 local_ray_dir = ray_dir;

    // TODO: check how to optimize this, there should be a way in optix to rotate coordinates
    float3 local_x = cyl.base_x;
    float3 local_z = cyl.base_z;
    float3 local_y = cross(local_z, local_x);

    local_ray_orig = make_float3(
        dot(local_ray_orig, local_x),
        dot(local_ray_orig, local_y),
        dot(local_ray_orig, local_z));
    local_ray_dir = make_float3(
        dot(local_ray_dir, local_x),
        dot(local_ray_dir, local_y),
        dot(local_ray_dir, local_z));

    // solve quadratic equation for intersection
    float A = local_ray_dir.x * local_ray_dir.x + local_ray_dir.z * local_ray_dir.z;
    float B = 2.0f * (local_ray_orig.x * local_ray_dir.x + local_ray_orig.z * local_ray_dir.z);
    float C = local_ray_orig.x * local_ray_orig.x + local_ray_orig.z * local_ray_orig.z - cyl.radius * cyl.radius;

    float determinant = B * B - 4.0f * A * C;

    if (determinant < 0.0f)
    {
        // No intersection
        return;
    }

    // Compute intersection distances
    float t1 = (-B - sqrtf(determinant)) / (2.0f * A);
    float t2 = (-B + sqrtf(determinant)) / (2.0f * A);

    float t = t1 > 0.0f ? t1 : t2; // Use the closer valid intersection
    if (t < ray_tmin || t > ray_tmax)
    {
        // Intersection is out of bounds
        return;
    }

    // Compute intersection point in local space
    float3 local_hit_point = local_ray_orig + t * local_ray_dir;

    // Check if the hit point is within the cylinder's height bounds
    if (fabsf(local_hit_point.y) > cyl.half_height)
    {
        // If t1 is invalid, try t2
        t = t2;
        local_hit_point = local_ray_orig + t * local_ray_dir;
        if (t < ray_tmin || t > ray_tmax || fabsf(local_hit_point.y) > cyl.half_height)
        {
            return; // Both intersections are out of bounds
        }
    }

    // Compute normal in local coordinates
    float3 local_normal = normalize(make_float3(local_hit_point.x, 0.0f, local_hit_point.z));

    // Transform normal back to world coordinates
    float3 world_normal = local_normal.x * local_x + local_normal.y * local_y + local_normal.z * local_z;

    // Compute the hit point in world space
    float3 world_hit_point = ray_orig + t * ray_dir;

    // Report intersection to OptiX
    optixReportIntersection(t,
                            0,
                            __float_as_uint(world_normal.x),
                            __float_as_uint(world_normal.y),
                            __float_as_uint(world_normal.z));
}

// ray cylinder intersection with top and bottom caps
// it can also be modeled as cylinder with two disks.
extern "C" __global__ void __intersection__cylinder_y_capped()
{
    const OptixCSP::GeometryDataST::Cylinder_Y &cyl = params.geometry_data_array[optixGetPrimitiveIndex()].getCylinder_Y();

    // Get ray information: origin, direction, and min/max distances over which ray should be tested
    const float3 ray_orig = optixGetWorldRayOrigin();
    const float3 ray_dir = normalize(optixGetWorldRayDirection());
    const float ray_tmin = optixGetRayTmin();
    const float ray_tmax = optixGetRayTmax();

    // Transform ray to the cylinder's local coordinate system
    float3 local_ray_orig = ray_orig - cyl.center;
    float3 local_ray_dir = ray_dir;

    // Transform using the cylinder's local basis
    float3 local_x = cyl.base_x;
    float3 local_z = cyl.base_z;
    float3 local_y = cross(local_z, local_x);

    local_ray_orig = make_float3(
        dot(local_ray_orig, local_x),
        dot(local_ray_orig, local_y),
        dot(local_ray_orig, local_z));
    local_ray_dir = make_float3(
        dot(local_ray_dir, local_x),
        dot(local_ray_dir, local_y),
        dot(local_ray_dir, local_z));

    // Solve quadratic equation for intersection with curved surface
    float A = local_ray_dir.x * local_ray_dir.x + local_ray_dir.z * local_ray_dir.z;
    float B = 2.0f * (local_ray_orig.x * local_ray_dir.x + local_ray_orig.z * local_ray_dir.z);
    float C = local_ray_orig.x * local_ray_orig.x + local_ray_orig.z * local_ray_orig.z - cyl.radius * cyl.radius;

    float determinant = B * B - 4.0f * A * C;

    float t_curved = ray_tmax + 1.0f; // Initialize to invalid
    if (determinant >= 0.0f)
    {
        // Compute intersection distances
        float t1 = (-B - sqrtf(determinant)) / (2.0f * A);
        float t2 = (-B + sqrtf(determinant)) / (2.0f * A);

        // Select the closest valid intersection within bounds
        if (t1 > ray_tmin && t1 < ray_tmax && fabsf(local_ray_orig.y + t1 * local_ray_dir.y) <= cyl.half_height)
        {
            t_curved = t1;
        }
        else if (t2 > ray_tmin && t2 < ray_tmax && fabsf(local_ray_orig.y + t2 * local_ray_dir.y) <= cyl.half_height)
        {
            t_curved = t2;
        }
    }

    // Check intersection with top and bottom caps
    float t_caps = ray_tmax + 1.0f;
    {
        // Bottom cap: y = -half_height
        if (fabsf(local_ray_dir.y) > 1e-6f) // Avoid division by zero
        {
            float t = (-cyl.half_height - local_ray_orig.y) / local_ray_dir.y;
            float2 hit_point = make_float2(local_ray_orig.x + t * local_ray_dir.x,
                                           local_ray_orig.z + t * local_ray_dir.z);
            if (t > ray_tmin && t < ray_tmax && dot(hit_point, hit_point) <= cyl.radius * cyl.radius)
            {
                t_caps = t;
            }
        }

        // Top cap: y = +half_height
        if (fabsf(local_ray_dir.y) > 1e-6f)
        {
            float t = (cyl.half_height - local_ray_orig.y) / local_ray_dir.y;
            float2 hit_point = make_float2(local_ray_orig.x + t * local_ray_dir.x,
                                           local_ray_orig.z + t * local_ray_dir.z);
            if (t > ray_tmin && t < ray_tmax && dot(hit_point, hit_point) <= cyl.radius * cyl.radius)
            {
                t_caps = fminf(t_caps, t);
            }
        }
    }

    // Use the closest valid intersection
    float t = fminf(t_curved, t_caps);
    if (t >= ray_tmax || t <= ray_tmin)
    {
        return; // No valid intersection
    }

    // Compute intersection point and normal
    float3 local_hit_point = local_ray_orig + t * local_ray_dir;
    float3 local_normal;

    if (t == t_curved)
    {
        // Hit on the curved surface
        local_normal = normalize(make_float3(local_hit_point.x, 0.0f, local_hit_point.z));
    }
    else
    {
        // Hit on one of the caps
        local_normal = make_float3(0.0f, signbit(local_hit_point.y) ? -1.0f : 1.0f, 0.0f);
    }

    // Transform normal back to world coordinates
    float3 world_normal = local_normal.x * local_x + local_normal.y * local_y + local_normal.z * local_z;

    // Compute world-space hit point
    float3 world_hit_point = ray_orig + t * ray_dir;

    // Report intersection to OptiX
    optixReportIntersection(
        t,
        0, // User-defined instance ID or custom data
        __float_as_uint(world_normal.x),
        __float_as_uint(world_normal.y),
        __float_as_uint(world_normal.z));
}

// intersection algorithm for a flat triangle based on "Fast, Minimum Storage Ray/Triangle Intersection" by M�ller and Trumbore (1997)
// code from here: https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
extern "C" __device__ __inline__ float _triangle_intersect(
    float3 p0, float3 edge1, float3 edge2,
    float3 ro, float3 rd)
{
    const float3 pvec = cross(rd, edge2);
    const float det = dot(edge1, pvec);

    // // Backface culling + parallel rejection
    // // (det must be strictly positive and not tiny)
    // const float eps = 1e-8f;
    // if (det <= eps) return -1.0f;

    // Parallel rejection
    // (det must be not tiny)
    const float eps = 1e-8f;
    if (fabs(det) <= eps)
        return -1.0f;

    const float inv_det = 1.0f / det;

    const float3 tvec = ro - p0;
    const float u = dot(tvec, pvec) * inv_det;
    if (u < 0.0f || u > 1.0f)
        return -1.0f;

    const float3 qvec = cross(tvec, edge1);
    const float v = dot(rd, qvec) * inv_det;
    if (v < 0.0f || (u + v) > 1.0f)
        return -1.0f;

    const float t = dot(edge2, qvec) * inv_det;

    return t;
}

// intersection algorithm for a flat triangle based on "Fast, Minimum Storage Ray/Triangle Intersection" by M�ller and Trumbore (1997)
// code from here: https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
extern "C" __global__ void __intersection__triangle_flat()
{
    const OptixCSP::GeometryDataST::Triangle_Flat &tri = params.geometry_data_array[optixGetPrimitiveIndex()].getTriangle_Flat();

    const float3 ro = optixGetObjectRayOrigin();
    const float3 rd = optixGetObjectRayDirection();

    const float t = _triangle_intersect(tri.v0, tri.e1, tri.e2, ro, rd);

    if (t < optixGetRayTmin() || t > optixGetRayTmax())
        return;

    float3 world_normal = tri.normal;

    optixReportIntersection(t, 0,
                            __float_as_uint(world_normal.x),
                            __float_as_uint(world_normal.y),
                            __float_as_uint(world_normal.z));
}

extern "C" __global__ void __intersection__quadrilateral_flat()
{
    const OptixCSP::GeometryDataST::Quadrilateral_Flat &quad = params.geometry_data_array[optixGetPrimitiveIndex()].getQuadrilateral_Flat();

    const float3 ro = optixGetObjectRayOrigin();
    const float3 rd = optixGetObjectRayDirection();

    const float3 p0 = quad.p0;
    const float3 p2 = quad.p2;
    float3 e1 = quad.p1 - p0;
    float3 e2 = quad.p3 - p0;

    float t = _triangle_intersect(p0, e1, e2, ro, rd);

    if (t < optixGetRayTmin() || t > optixGetRayTmax())
    {
        e1 = quad.p1 - p2;
        e2 = quad.p3 - p2;
        t = _triangle_intersect(p2, e1, e2, ro, rd);
    }

    if (t < optixGetRayTmin() || t > optixGetRayTmax())
        return;

    float3 world_normal = quad.normal;

    optixReportIntersection(t, 0,
                            __float_as_uint(world_normal.x),
                            __float_as_uint(world_normal.y),
                            __float_as_uint(world_normal.z));
}

extern "C" __global__ void __intersection__circle_flat()
{
    const OptixCSP::GeometryDataST::Circle_Flat &circ = params.geometry_data_array[optixGetPrimitiveIndex()].getCircle_Flat();

    // Get ray information: origin, direction, and min/max distances over which ray should be tested
    const float3 ray_orig = optixGetWorldRayOrigin();
    const float3 ray_dir = optixGetWorldRayDirection();
    const float ray_tmin = optixGetRayTmin(), ray_tmax = optixGetRayTmax();

    // // Compute ray intersection point
    // float3 n  = make_float3( circ.plane );
    // float  dt = dot( ray_dir, n );
    // // Compute distance t (point of intersection) along ray direction from ray origin
    // float  t  = ( circ.plane.w - dot( n, ray_orig ) ) / dt;
    float t = ray_distance_to_plane(ray_orig, ray_dir, circ.plane);
    const float4 n = circ.plane;

    // Verify intersection distance and Report ray intersection point
    if (t > ray_tmin && t < ray_tmax)
    {
        // Since everything is in global coordinates (e.g., the ray intersection coordinates),
        // and the circle is rotationally symmetric, we don't need to worry about
        // any rotation of the circle in local coordinates
        float3 p = ray_orig + ray_dir * t;
        float d = length(p - circ.center);
        if (d <= circ.r)
        {
            optixReportIntersection(t,
                                    0,
                                    __float_as_uint(n.x),
                                    __float_as_uint(n.y),
                                    __float_as_uint(n.z));
        }
    }
}

extern "C" __global__ void __intersection__hexagon_flat()
{
    const OptixCSP::GeometryDataST::Hexagon_Flat &hex = params.geometry_data_array[optixGetPrimitiveIndex()].getHexagon_Flat();

    // Get ray information: origin, direction, and min/max distances over which ray should be tested
    const float3 ray_orig = optixGetWorldRayOrigin();
    const float3 ray_dir = optixGetWorldRayDirection();
    const float ray_tmin = optixGetRayTmin(), ray_tmax = optixGetRayTmax();

    float t = ray_distance_to_plane(ray_orig, ray_dir, hex.plane);
    const float4 n = hex.plane;

    // Verify intersection distance and Report ray intersection point
    if (t > ray_tmin && t < ray_tmax)
    {
        bool is_in = false;
        float3 p = ray_orig + ray_dir * t - hex.center;
        // Project onto the local x and y axes which are unit vectors
        const float px = dot(p, hex.x_axis);
        const float py = dot(p, hex.y_axis);
        const float s = hex.s;
        const float xl = 0.5f * s;
        const float yl = 0.5f * sqrtf(3.0f) * s;
        if (-xl <= px && px <= xl && -yl <= py && py <= yl)
        {
            // Center
            is_in = true;
        }
        else if (-s <= px && px < -xl)
        {
            // Left side
            float y1 = sqrtf(3.0f) * (px + s);
            float y2 = -y1;
            if (y2 <= py && py <= y1)
            {
                is_in = true;
            }
        }
        else if (xl < px && px <= s)
        {
            // Right side
            float y1 = sqrtf(3.0f) * (px - s);
            float y2 = -y1;
            if (y1 <= py && py <= y2)
            {
                is_in = true;
            }
        }

        if (is_in)
        {
            optixReportIntersection(t,
                                    0,
                                    __float_as_uint(n.x),
                                    __float_as_uint(n.y),
                                    __float_as_uint(n.z));
        }
    }
}

extern "C" __global__ void __intersection__annulus_flat()
{
    const OptixCSP::GeometryDataST::Annulus_Flat &anf = params.geometry_data_array[optixGetPrimitiveIndex()].getAnnulus_Flat();

    // Get ray information: origin, direction, and min/max distances over which ray should be tested
    const float3 ray_orig = optixGetWorldRayOrigin();
    const float3 ray_dir = optixGetWorldRayDirection();
    const float ray_tmin = optixGetRayTmin(), ray_tmax = optixGetRayTmax();

    float t = ray_distance_to_plane(ray_orig, ray_dir, anf.plane);
    const float4 n = anf.plane;

    // Verify intersection distance and Report ray intersection point
    if (t > ray_tmin && t < ray_tmax)
    {
        float3 p = ray_orig + ray_dir * t - anf.center;
        float d = length(p);
        if (anf.ri <= d && d <= anf.ro)
        {
            float px = dot(p, anf.x_axis);
            float py = dot(p, anf.y_axis);
            float theta = atan2f(py, px);
            if (fabsf(theta) <= 0.5f * anf.arc)
            {
                optixReportIntersection(t,
                                        0,
                                        __float_as_uint(n.x),
                                        __float_as_uint(n.y),
                                        __float_as_uint(n.z));
            }
        }
    }
}

// -----------------------------------------------------------------------
// Shared helpers for parabolic surface intersections.
//
// All parabolic surfaces share the same quadric equation:
//   z = (cx/2)*x^2 + (cy/2)*y^2
// in a local frame (center, x_ax, y_ax, n=cross(x_ax,y_ax)).
// The three helpers below factor out the ray transform, quadratic solve,
// and normal computation. Each kernel only supplies the aperture test.
// -----------------------------------------------------------------------

// Transform a world-space ray into the local parabolic frame.
// Outputs the frame normal n = normalize(cross(x_ax, y_ax)) and
// the local ray origin (ox,oy,oz) and direction (dx,dy,dz).
extern "C" __device__ __inline__ void parabolic_ray_to_local(
    const float3 &ray_orig, const float3 &ray_dir,
    const float3 &center,
    const float3 &x_ax, const float3 &y_ax,
    float3 &n,
    float &ox, float &oy, float &oz,
    float &dx, float &dy, float &dz)
{
    n = normalize(cross(x_ax, y_ax));
    const float3 d = ray_orig - center;
    ox = dot(d, x_ax);
    oy = dot(d, y_ax);
    oz = dot(d, n);
    dx = dot(ray_dir, x_ax);
    dy = dot(ray_dir, y_ax);
    dz = dot(ray_dir, n);
}

// Solve A*t^2 + B*t + C = 0 for the paraboloid-ray intersection and return
// up to two hits within [ray_tmin, ray_tmax], ordered by ascending t.
// Returns the number of valid hits (0, 1, or 2).
// t_out[i], lx_out[i], ly_out[i] give the ray parameter and local (x,y) of each hit.
extern "C" __device__ __inline__ int parabolic_solve(
    float ox, float oy, float oz,
    float dx, float dy, float dz,
    float cx, float cy,
    float ray_tmin, float ray_tmax,
    float t_out[2], float lx_out[2], float ly_out[2])
{
    const float A = 0.5f * cx * dx * dx + 0.5f * cy * dy * dy;
    const float B = cx * ox * dx + cy * oy * dy - dz;
    const float C = 0.5f * cx * ox * ox + 0.5f * cy * oy * oy - oz;

    const float eps = 1e-6f;
    int count = 0;

    if (fabsf(A) < eps)
    {
        if (fabsf(B) > eps)
        {
            const float t = -C / B;
            if (t >= ray_tmin && t <= ray_tmax)
            {
                t_out[0] = t;
                lx_out[0] = ox + t * dx;
                ly_out[0] = oy + t * dy;
                count = 1;
            }
        }
    }
    else
    {
        const float discr = B * B - 4.0f * A * C;
        if (discr >= 0.0f)
        {
            const float sq = sqrtf(discr);
            // A > 0 (physical curvature), so ta <= tb is guaranteed.
            const float ta = -0.5f * (B + sq) / A;
            const float tb = -0.5f * (B - sq) / A;
            if (ta >= ray_tmin && ta <= ray_tmax)
            {
                t_out[count] = ta;
                lx_out[count] = ox + ta * dx;
                ly_out[count] = oy + ta * dy;
                ++count;
            }
            if (tb >= ray_tmin && tb <= ray_tmax)
            {
                t_out[count] = tb;
                lx_out[count] = ox + tb * dx;
                ly_out[count] = oy + tb * dy;
                ++count;
            }
        }
    }
    return count;
}

// Compute the world-space unit normal at a parabolic surface hit.
// x_hit, y_hit : local coordinates of the hit point
// cx, cy       : curvature parameters
// x_ax, y_ax   : local frame unit vectors
// n            : normalize(cross(x_ax, y_ax))
extern "C" __device__ __inline__ float3 parabolic_world_normal(
    float x_hit, float y_hit,
    float cx, float cy,
    const float3 &x_ax, const float3 &y_ax, const float3 &n)
{
    const float3 N_local = make_float3(-cx * x_hit, -cy * y_hit, 1.0f);
    return N_local.x * x_ax + N_local.y * y_ax + N_local.z * n;
}

// -----------------------------------------------------------------------

// Parabolic surface, rectangle aperture.
// z = (curv_x/2)*x^2 + (curv_y/2)*y^2, aperture: |x| <= L1/2, |y| <= L2/2.
extern "C" __global__ void __intersection__rectangle_parabolic()
{
    const OptixCSP::GeometryDataST::Rectangle_Parabolic &rect =
        params.geometry_data_array[optixGetPrimitiveIndex()].getRectangleParabolic();

    const float3 ray_orig = optixGetWorldRayOrigin();
    const float3 ray_dir = optixGetWorldRayDirection();
    const float ray_tmin = optixGetRayTmin();
    const float ray_tmax = optixGetRayTmax();

    // Recover unit edge vectors and half-lengths from the stored reciprocal vectors.
    // rect.v1 = original_v1 / dot(original_v1, original_v1), so |rect.v1| = 1/L1.
    const float L1 = 1.0f / length(rect.v1);
    const float L2 = 1.0f / length(rect.v2);
    const float3 e1 = rect.v1 * L1;
    const float3 e2 = rect.v2 * L2;
    const float3 center = rect.anchor + (0.5f * L1) * e1 + (0.5f * L2) * e2;

    float3 n;
    float ox, oy, oz, dx, dy, dz;
    parabolic_ray_to_local(ray_orig, ray_dir, center, e1, e2,
                           n, ox, oy, oz, dx, dy, dz);

    float ts[2], lxs[2], lys[2];
    const int nc = parabolic_solve(ox, oy, oz, dx, dy, dz,
                                   rect.curv_x, rect.curv_y,
                                   ray_tmin, ray_tmax, ts, lxs, lys);

    const float half_L1 = 0.5f * L1;
    const float half_L2 = 0.5f * L2;
    for (int i = 0; i < nc; ++i)
    {
        if (lxs[i] >= -half_L1 && lxs[i] <= half_L1 &&
            lys[i] >= -half_L2 && lys[i] <= half_L2)
        {
            const float3 wn = parabolic_world_normal(lxs[i], lys[i],
                                                     rect.curv_x, rect.curv_y,
                                                     e1, e2, n);
            optixReportIntersection(ts[i], 0,
                                    __float_as_uint(wn.x),
                                    __float_as_uint(wn.y),
                                    __float_as_uint(wn.z));
            return;
        }
    }
}

// Parabolic surface, circle aperture.
// z = (cx/2)*x^2 + (cy/2)*y^2, aperture: x^2 + y^2 <= radius^2.
extern "C" __global__ void __intersection__circle_parabolic()
{
    const OptixCSP::GeometryDataST::Circle_Parabolic &circp =
        params.geometry_data_array[optixGetPrimitiveIndex()].getCircle_Parabolic();

    const float3 ray_orig = optixGetWorldRayOrigin();
    const float3 ray_dir = optixGetWorldRayDirection();
    const float ray_tmin = optixGetRayTmin();
    const float ray_tmax = optixGetRayTmax();

    float3 n;
    float ox, oy, oz, dx, dy, dz;
    parabolic_ray_to_local(ray_orig, ray_dir, circp.center, circp.x_axis, circp.y_axis,
                           n, ox, oy, oz, dx, dy, dz);

    float ts[2], lxs[2], lys[2];
    const int nc = parabolic_solve(ox, oy, oz, dx, dy, dz,
                                   circp.cx, circp.cy,
                                   ray_tmin, ray_tmax, ts, lxs, lys);

    const float r2 = circp.radius * circp.radius;
    for (int i = 0; i < nc; ++i)
    {
        if (lxs[i] * lxs[i] + lys[i] * lys[i] <= r2)
        {
            const float3 wn = parabolic_world_normal(lxs[i], lys[i],
                                                     circp.cx, circp.cy,
                                                     circp.x_axis, circp.y_axis, n);
            optixReportIntersection(ts[i], 0,
                                    __float_as_uint(wn.x),
                                    __float_as_uint(wn.y),
                                    __float_as_uint(wn.z));
            return;
        }
    }
}

extern "C" __global__ void __intersection__hexagon_parabolic()
{
    const OptixCSP::GeometryDataST::Hexagon_Parabolic &hexp =
        params.geometry_data_array[optixGetPrimitiveIndex()].getHexagon_Parabolic();

    const float3 ray_orig = optixGetWorldRayOrigin();
    const float3 ray_dir = optixGetWorldRayDirection();
    const float ray_tmin = optixGetRayTmin();
    const float ray_tmax = optixGetRayTmax();

    float3 n;
    float ox, oy, oz, dx, dy, dz;
    parabolic_ray_to_local(ray_orig, ray_dir,
                           hexp.center, hexp.x_axis, hexp.y_axis,
                           n, ox, oy, oz, dx, dy, dz);

    float ts[2], lxs[2], lys[2];
    const int nc = parabolic_solve(ox, oy, oz, dx, dy, dz,
                                   hexp.cx, hexp.cy,
                                   ray_tmin, ray_tmax,
                                   ts, lxs, lys);

        for (int i = 0; i < nc; ++i)
    {
        bool is_in = false;
        float3 p = ray_orig + ray_dir * ts[i] - hexp.center;
        // Project onto the local x and y axes which are unit vectors
        const float px = dot(p, hexp.x_axis);
        const float py = dot(p, hexp.y_axis);
        const float s = hexp.s;
        const float xl = 0.5f * s;
        const float yl = 0.5f * sqrtf(3.0f) * s;
        if (-xl <= px && px <= xl && -yl <= py && py <= yl)
        {
            // Center
            is_in = true;
        }
        else if (-s <= px && px < -xl)
        {
            // Left side
            float y1 = sqrtf(3.0f) * (px + s);
            float y2 = -y1;
            if (y2 <= py && py <= y1)
            {
                is_in = true;
            }
        }
        else if (xl < px && px <= s)
        {
            // Right side
            float y1 = sqrtf(3.0f) * (px - s);
            float y2 = -y1;
            if (y1 <= py && py <= y2)
            {
                is_in = true;
            }
        }

        if (is_in)
        {
            const float3 wn = parabolic_world_normal(lxs[i], lys[i],
                                                     hexp.cx, hexp.cy,
                                                     hexp.x_axis, hexp.y_axis, n);

            optixReportIntersection(ts[i],
                                    0,
                                    __float_as_uint(wn.x),
                                    __float_as_uint(wn.y),
                                    __float_as_uint(wn.z));

            return;
        }
    }
}

extern "C" __global__ void __intersection__triangle_parabolic()
{
}

extern "C" __global__ void __intersection__annulus_parabolic()
{
}

extern "C" __global__ void __intersection__quadrilateral_parabolic()
{
}
