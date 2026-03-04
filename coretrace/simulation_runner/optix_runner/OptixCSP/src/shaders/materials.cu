#include <optix_device.h>
#include <vector_types.h>
#include "Soltrace.h"
#include <stdio.h>
#include "MaterialDataST.h"
#include "soltrace_constants.h"
#include "curand_kernel.h"

// Launch parameters for soltrace
extern "C"
{
    __constant__ OptixCSP::LaunchParams params;
}

namespace OptixCSP
{
    static __device__ __inline__ OptixCSP::PerRayData getPayload()
    {
        OptixCSP::PerRayData prd;
        prd.ray_path_index = optixGetPayload_0();
        prd.depth = optixGetPayload_1();
        return prd;
    }

    static __device__ __inline__ void setPayload(const OptixCSP::PerRayData &prd)
    {
        optixSetPayload_0(prd.ray_path_index);
        optixSetPayload_1(prd.depth);
    }

    // // 32-bit avalanche mix (fast, good diffusion)
    // static __device__ __inline__ float rng_uniform(uint32_t x)
    // {
    //     x ^= x >> 16;
    //     x *= 0x85EBCA6Bu;
    //     x ^= x >> 13;
    //     x *= 0xC2B2AE35u;
    //     x ^= x >> 16;
    //     return float(x >> 8) * (1.0f / 16777216.0f); // Scale to [0, 1)
    // }
}

// Assumes that v is a unit vector
extern "C" __device__ __inline__ float3 orthonormal_vector(float3 v)
{
    // TODO: Need to handle w = c * v case...
    float3 u;
    if (fabs(v.x) < 0.9f)
    {
        // Code does the following:
        // w = make_float3(1.0f, 0.0f, 0.0f);
        // u = cross(v, w);
        u = make_float3(0.0f, v.z, -v.y);
    }
    else
    {
        // Code does the following:
        // w = make_float3(0.0f, 1.0f, 0.0f);
        // u = cross(v, w);
        u = make_float3(-v.z, 0.0f, v.x);
    }    
    return normalize(u);
}

// Add perturbation ortogonal to given vector. Perturbation is uniform over
// a disk of radius a centered at the vector n. Returned vector is a
// unit vector.
extern "C" __device__ float3 apply_uniform_errors(float a,
                                                  float3 v,
                                                  OptixCSP::PerRayData &prd)
{
    curandState local_rng = params.rng_states[prd.ray_path_index];
    float3 eta = orthonormal_vector(v);
    float3 xi = cross(v, eta);
    float phi = 2.0f * M_PIf * curand_uniform(&local_rng);
    float r = a * sqrtf(curand_uniform(&local_rng));
    eta = r * cosf(phi) * eta + r * sinf(phi) * xi;
    params.rng_states[prd.ray_path_index] = local_rng;
    return normalize(v + eta);
}

// Add perturbation orthogonal to given vector. Magnitude is Gaussian with
// standard deviation sigma and the angle is uniform over [-pi, pi).
// Returned vector is a unit vector.
extern "C" __device__ float3 apply_gaussian_errors(float sigma,
                                                   float3 n,
                                                   OptixCSP::PerRayData &prd)
{
    curandState local_rng = params.rng_states[prd.ray_path_index];
    float3 eta = orthonormal_vector(n);
    const float3 xi = cross(n, eta);
    float phi = curand_normal(&local_rng);
    float r = curand_normal(&local_rng);
    r = sigma * sqrtf(r * r + phi * phi);
    phi = 2.0f * M_PIf * curand_uniform(&local_rng);
    eta = r * cosf(phi) * eta + r * sinf(phi) * xi;
    params.rng_states[prd.ray_path_index] = local_rng;
    return normalize(n + eta);

    // // Could also do the below but it is slower in CPU trials...
    // curandState local_rng = params.rng_states[prd.ray_path_index];
    // float3 eta = orthonormal_vector(v);
    // float3 xi = cross(v, eta);
    // float phi = 2.0f * M_PIf * curand_uniform(&local_rng);
    // float r = sigma * curand_normal(&local_rng);
    // eta = r * cosf(phi) * eta + r * sinf(phi) * xi;
    // params.rng_states[prd.ray_path_index] = local_rng;
    // return normalize(n + eta);
}

extern "C" __global__ void __closesthit__mirror()
{
    // Determine if we are using optical errors
    const bool optical_errors = params.optical_errors;

    // Fetch the normal vector from the hit attributes passed by OptiX
    float3 object_normal = make_float3(__uint_as_float(optixGetAttribute_0()), __uint_as_float(optixGetAttribute_1()),
                                       __uint_as_float(optixGetAttribute_2()));
    // Transform the object-space normal to world space using OptiX built-in function
    float3 world_normal = normalize(optixTransformNormalFromObjectToWorldSpace(object_normal));

    // Compute the facing normal, which handles the direction of the normal based on the incoming ray direction
    const float3 ray_dir = optixGetWorldRayDirection();
    float3 ffnormal = faceforward(world_normal, -ray_dir, world_normal);

    // Get the incoming ray's origin, direction, and max t (intersection distance)
    const float3 ray_orig = optixGetWorldRayOrigin();
    const float ray_t = optixGetRayTmax();

    // Determine hit direction (front or back)
    const float dot_nd = dot(ray_dir, world_normal);
    const bool hit_front_face = (dot_nd < 0.0f);

    // Compute the hit point of the ray using its origin and direction, scaled by the intersection distance (ray_t)
    const float3 hit_point = ray_orig + ray_t * ray_dir;

    OptixCSP::PerRayData prd = OptixCSP::getPayload();
    const int new_depth = prd.depth + 1; // Increment the ray depth for recursive tracing

    // we have two scenarios here
    // if we use refraction, then we look at transmissivity to determine if the ray will refract
    // or get obsorbed. otherwise, it will get reflected.
    float3 new_dir;
    bool absorbed = false; // determine whether the ray is absorbed or not, this is montecarlo based, should be applied to reflection and refraction
    uint8_t hit_type = OptixCSP::HitType::HIT_UNASSIGNED;

    // Get optical properties
    OptixCSP::MaterialData material = hit_front_face ? params.material_data_array_front[optixGetPrimitiveIndex()]
                                                     : params.material_data_array_back[optixGetPrimitiveIndex()];
    const float transmissivity = material.transmissivity;
    const bool use_transmissivity = material.use_refraction;
    const float reflectivity = material.reflectivity;
    const float normal_sigma = 1e-3f * material.slope_error;
    const float spec_sigma = material.specularity_error;
    const uint8_t error_type = material.optical_dist;

    // Surface normal (macro-surface) errors
    if (optical_errors)
    {
        if (error_type == OptixCSP::OpticalDistribution::OPT_GAUSSIAN)
        {
            ffnormal = apply_gaussian_errors(normal_sigma, ffnormal, prd);
        }
        else if (error_type == OptixCSP::OpticalDistribution::OPT_PILLBOX)
        {
            ffnormal = apply_uniform_errors(normal_sigma, ffnormal, prd);
        }
        else
        {
            ; // Intentional no-op
        }
    }

    // now we figure out the random number to determine if the ray is absorbed or refracted
    // float xi = OptixCSP::rng_uniform(prd); // random number in [0,1)
    const float xi = curand_uniform(&params.rng_states[prd.ray_path_index]);

    if (use_transmissivity)
    {
        if (xi > transmissivity)
        {
            absorbed = true;
            hit_type = OptixCSP::HitType::HIT_ABSORB;
            // printf("ray is absorbed! ray index is %d, depth %d\n", prd.ray_path_index, prd.depth);
        } // ray is absorbed
        else
        {
            new_dir = refract(ray_dir, ffnormal);
            hit_type = OptixCSP::HitType::HIT_TRANSMIT;
        }
    }
    else
    {
        if (xi > reflectivity)
        {
            absorbed = true;
            hit_type = OptixCSP::HitType::HIT_ABSORB;
            // printf("ray is absorbed! ray index is %d, depth %d\n", prd.ray_path_index, prd.depth);
        } // ray is absorbed
        else
        {
            new_dir = reflect(ray_dir, ffnormal);
            hit_type = OptixCSP::HitType::HIT_REFLECT;
        }
    }

    if (optical_errors)
    {
        // Optical (micro-surface) errors
        if (error_type == OptixCSP::OpticalDistribution::OPT_GAUSSIAN)
        {
            new_dir = apply_gaussian_errors(spec_sigma, new_dir, prd);
        }
        else if (error_type == OptixCSP::OpticalDistribution::OPT_PILLBOX)
        {
            new_dir = apply_uniform_errors(spec_sigma, new_dir, prd);
        }
        else
        {
            ; // Intentional no-op
        }
    }

    ////////////////////////////////////////////////////////////////
    // // Alternative to the above doing this twice is to combine it
    // // into a single spot...may not work as intended for pillbox?
    // // Would be more efficient since we could combine the two
    // // error values during setup and then we would only need one
    // // field and we would avoid a whole if clause.
    // const float sigma = sqrt(4.0f * normal_sigma + spec_sigma);
    // if (optical_errors)
    // {
    //     // Optical (micro-surface) errors
    //     if (error_type == OptixCSP::OpticalDistribution::OPT_GAUSSIAN)
    //     {
    //         new_dir = apply_gaussian_errors(sigma, new_dir, prd);
    //     }
    //     else if (error_type == OptixCSP::OpticalDistribution::OPT_PILLBOX)
    //     {
    //         new_dir = apply_uniform_errors(sigma, new_dir, prd);
    //     }
    //     else
    //     {
    //         ; // Intentional no-op
    //     }
    // }
    /////////////////////////////////////////////////////////////////

    // Check if the maximum recursion depth has not been reached
    if (new_depth < params.max_depth)
    {

        // Get buffer slot
        const int slot = params.max_depth * prd.ray_path_index + new_depth;

        // Store the hit point in the hit point buffer (used for visualization or further calculations)
        params.hit_point_buffer[slot] = make_float4(new_depth, hit_point);

        // Store element id
        const int32_t elementId = params.geometry_data_array[optixGetPrimitiveIndex()].id;
        params.element_id_buffer[slot] = elementId;

        // Store hit type
        params.hit_type_buffer[slot] = hit_type;

        // Store the reflected direction in its buffer (used for visualization or further calculations)
        /*
        params.reflected_dir_buffer[params.max_depth * prd.ray_path_index + new_depth] = make_float4(1.0f, reflected_dir);
        */

        // Trace the reflected ray
        prd.depth = new_depth;
        if (!absorbed)
        {
            // printf("launching rays that are not absorbed: %d, depth, %d\n", prd.ray_path_index, prd.depth - 1);
            optixTrace(
                params.handle,                                        // The handle to the acceleration structure
                hit_point,                                            // The starting point of the reflected ray
                new_dir,                                              // The direction of the reflected ray
                0.01f,                                                // A small offset to avoid self-intersection (shadow acne)
                1e16f,                                                // Maximum distance the ray can travel
                0.0f,                                                 // Ray time (used for time-dependent effects)
                OptixVisibilityMask(1),                               // Visibility mask (defines what the ray can interact with)
                OPTIX_RAY_FLAG_NONE,                                  // Ray flags (no special flags for now)
                OptixCSP::RAY_TYPE_RADIANCE,                          // Use the radiance ray type
                OptixCSP::RAY_TYPE_COUNT,                             // Total number of ray types
                OptixCSP::RAY_TYPE_RADIANCE,                          // The ray type's offset into the SBT
                reinterpret_cast<unsigned int &>(prd.ray_path_index), // Pass the ray path index
                reinterpret_cast<unsigned int &>(prd.depth)           // Pass the updated depth
            );
        }
        else
        {
            // printf("ray %d is absorbed, terminate! depth = %d\n", prd.ray_path_index, prd.depth - 1);
            prd.depth = params.max_depth; // terminate the ray by setting depth to max depth
        }
    }

    setPayload(prd);
}

extern "C" __global__ void __closesthit__receiver()
{
    float3 object_normal = make_float3(__uint_as_float(optixGetAttribute_0()),
                                       __uint_as_float(optixGetAttribute_1()),
                                       __uint_as_float(optixGetAttribute_2()));

    // Incident ray properties (origin, direction, and max t distance)
    const float3 ray_orig = optixGetWorldRayOrigin();
    const float3 ray_dir = optixGetWorldRayDirection();
    const float ray_t = optixGetRayTmax();
    OptixCSP::PerRayData prd = OptixCSP::getPayload();

    // printf("ray id hitting the receiver: %d, depth %d\n", prd.ray_path_index, prd.depth);

    // Compute the normal of the receiver and dot with ray direction to determine which side was hit
    const float dot_product = dot(ray_dir, object_normal);

    float3 hit_point = ray_orig + ray_t * ray_dir;

    const int new_depth = prd.depth + 1;

    // Check if the ray hits the receiver surface (dot product negative means ray is hitting the front face)
    if (dot_product < 0.0f)
    {
        if (new_depth < params.max_depth)
        {
            const int slot = params.max_depth * prd.ray_path_index + new_depth;
            params.hit_point_buffer[slot] = make_float4(new_depth, hit_point);
            const int32_t elementId = params.geometry_data_array[optixGetPrimitiveIndex()].id;
            params.element_id_buffer[slot] = elementId;
            params.hit_type_buffer[slot] = OptixCSP::HitType::HIT_ABSORB;

            prd.depth = new_depth;
        }
    }

    setPayload(prd);
}

extern "C" __global__ void __closesthit__receiver__cylinder__y()
{
    //// Retrieve the hit group data and access the parallelogram geometry
    // const OptixCSP::HitGroupData* sbt_data = reinterpret_cast<OptixCSP::HitGroupData*>(optixGetSbtDataPointer());

    const OptixCSP::GeometryDataST::Cylinder_Y &cyl = params.geometry_data_array[optixGetPrimitiveIndex()].getCylinder_Y();

    /*
    float3 object_normal = make_float3( __uint_as_float( optixGetAttribute_0() ), __uint_as_float( optixGetAttribute_1() ),
                                        __uint_as_float( optixGetAttribute_2() ) );
    float3 world_normal  = normalize( optixTransformNormalFromObjectToWorldSpace( object_normal ) );
    float3 ffnormal      = faceforward( world_normal, -optixGetWorldRayDirection(), world_normal );
    */

    // Incident ray properties (origin, direction, and max t distance)
    const float3 ray_orig = optixGetWorldRayOrigin();
    const float3 ray_dir = optixGetWorldRayDirection();
    const float ray_t = optixGetRayTmax();

    // Compute the normal of the receiver and dot with ray direction to determine which side was hit
    // TODO: this normal is hard coded based on how the geometry was defined, need to make more robust
    // const float3 receiver_normal = cyl.base_x;
    // const float dot_product = dot(ray_dir, receiver_normal);

    float3 hit_point = ray_orig + ray_t * ray_dir;

    OptixCSP::PerRayData prd = OptixCSP::getPayload();
    const int new_depth = prd.depth + 1;

    // Check if the ray hits the receiver surface (dot product negative means ray is hitting the front face)
    // if (dot_product < 0.0f) {
    if (new_depth < params.max_depth)
    {
        const int slot = params.max_depth * prd.ray_path_index + new_depth;
        params.hit_point_buffer[slot] = make_float4(new_depth, hit_point);
        prd.depth = new_depth;
        const int32_t elementId = params.geometry_data_array[optixGetPrimitiveIndex()].id;
        params.element_id_buffer[slot] = elementId;
        params.hit_type_buffer[slot] = OptixCSP::HitType::HIT_ABSORB;
    }
    //}

    setPayload(prd);
}

// Closest-hit for a parabolic mirror surface.
// This function is nearly identical to __closesthit__mirror(), but is intended to be
// used when the surface is a parabolic mirror. The intersection shader (e.g.
// __intersection__rectangle_parabolic) reports a normal that already accounts for the curvature.
extern "C" __global__ void __closesthit__mirror__parabolic()
{
    // Optionally, you can access material data if needed:
    // const OptixCSP::HitGroupData* sbt_data = reinterpret_cast<OptixCSP::HitGroupData*>( optixGetSbtDataPointer() );
    // const MaterialData::Mirror& mirror = sbt_data->material_data.mirror;

    // Determine if we are using optical errors
    const bool optical_errors = params.optical_errors;

    // Retrieve the hit normal from the attributes.
    // The intersection shader for the parabolic surface reported the normal (using float3_as_args)
    // in the hit attributes. In many cases this normal is already in world space.
    float3 object_normal = make_float3(__uint_as_float(optixGetAttribute_0()),
                                       __uint_as_float(optixGetAttribute_1()),
                                       __uint_as_float(optixGetAttribute_2()));
    // If the normal reported by the intersection shader is in object space you can transform it;
    // if it is already in world space, this call may be omitted.
    float3 world_normal = normalize(optixTransformNormalFromObjectToWorldSpace(object_normal));

    // Ensure that the normal is facing the incoming ray.
    float3 ffnormal = faceforward(world_normal, -optixGetWorldRayDirection(), world_normal);

    // Retrieve ray data.
    const float3 ray_orig = optixGetWorldRayOrigin();
    const float3 ray_dir = optixGetWorldRayDirection();
    const float ray_t = optixGetRayTmax();

    // Compute the hit point.
    const float3 hit_point = ray_orig + ray_t * ray_dir;

    // Retrieve per ray payload.
    OptixCSP::PerRayData prd = OptixCSP::getPayload();
    const int new_depth = prd.depth + 1; // Increase recursion depth.

    // we have two scenarios here
    // if we use refraction, then we look at transmissivity to determine if the ray will refract
    // or get obsorbed. otherwise, it will get reflected.
    float3 new_dir;
    bool absorbed = false; // determine whether the ray is absorbed or not, this is montecarlo based, should be applied to reflection and refraction
    uint8_t hit_type = OptixCSP::HitType::HIT_UNASSIGNED;

    // Determine hit direction (front or back)
    const float dot_nd = dot(ray_dir, world_normal);
    const bool hit_front_face = (dot_nd < 0.0f);

    // Get optical properties
    OptixCSP::MaterialData material = hit_front_face ? params.material_data_array_front[optixGetPrimitiveIndex()]
                                                     : params.material_data_array_back[optixGetPrimitiveIndex()];
    const float transmissivity = material.transmissivity;
    const bool use_transmissivity = material.use_refraction;
    const float reflectivity = material.reflectivity;
    const float normal_sigma = 1e-3f * material.slope_error;
    const float spec_sigma = material.specularity_error;
    const uint8_t error_type = material.optical_dist;

    if (optical_errors)
    {
        // Surface normal (macro-surface) errors
        if (error_type == OptixCSP::OpticalDistribution::OPT_GAUSSIAN)
        {
            ffnormal = apply_gaussian_errors(normal_sigma, ffnormal, prd);
        }
        else if (error_type == OptixCSP::OpticalDistribution::OPT_PILLBOX)
        {
            ffnormal = apply_uniform_errors(normal_sigma, ffnormal, prd);
        }
        else
        {
            ; // Intentional no-op
        }
    }

    // now we figure out the random number to determine if the ray is absorbed or refracted
    // float xi = OptixCSP::rng_uniform(prd); // random number in [0,1)
    const float xi = curand_uniform(&params.rng_states[prd.ray_path_index]);

    if (use_transmissivity)
    {
        if (xi > transmissivity)
        {
            absorbed = true;
            hit_type = OptixCSP::HitType::HIT_ABSORB;
            // printf("ray is absorbed! ray index is %d, depth %d\n", prd.ray_path_index, prd.depth);
        } // ray is absorbed
        else
        {
            new_dir = refract(ray_dir, ffnormal);
            hit_type = OptixCSP::HitType::HIT_TRANSMIT;
        }
    }
    else
    {
        if (xi > reflectivity)
        {
            absorbed = true;
            hit_type = OptixCSP::HitType::HIT_ABSORB;
            // printf("ray is absorbed! ray index is %d, depth %d\n", prd.ray_path_index, prd.depth);
        } // ray is absorbed
        else
        {
            new_dir = reflect(ray_dir, ffnormal);
            hit_type = OptixCSP::HitType::HIT_REFLECT;
        }
    }

    if (optical_errors)
    {
        // Optical (micro-surface) errors
        if (error_type == OptixCSP::OpticalDistribution::OPT_GAUSSIAN)
        {
            new_dir = apply_gaussian_errors(spec_sigma, new_dir, prd);
        }
        else if (error_type == OptixCSP::OpticalDistribution::OPT_PILLBOX)
        {
            new_dir = apply_uniform_errors(spec_sigma, new_dir, prd);
        }
        else
        {
            ; // Intentional no-op
        }
    }

    // If the new depth is below the maximum, trace the reflected ray.
    if (new_depth < params.max_depth)
    {
        // Save the hit point (for visualization or further processing).
        const int slot = params.max_depth * prd.ray_path_index + new_depth;
        params.hit_point_buffer[slot] = make_float4(new_depth, hit_point);
        const int32_t elementId = params.geometry_data_array[optixGetPrimitiveIndex()].id;
        params.element_id_buffer[slot] = elementId;
        params.hit_type_buffer[slot] = hit_type;

        prd.depth = new_depth;
        if (!absorbed)
        {
            optixTrace(
                params.handle,                                        // Acceleration structure handle.
                hit_point,                                            // Ray origin.
                new_dir,                                              // Ray direction.
                0.01f,                                                // Minimum t to avoid self-intersection.
                1e16f,                                                // Maximum t.
                0.0f,                                                 // Ray time.
                OptixVisibilityMask(1),                               // Visibility mask.
                OPTIX_RAY_FLAG_NONE,                                  // Ray flags.
                OptixCSP::RAY_TYPE_RADIANCE,                          // Ray type.
                OptixCSP::RAY_TYPE_COUNT,                             // Number of ray types.
                OptixCSP::RAY_TYPE_RADIANCE,                          // SBT offset for this ray type.
                reinterpret_cast<unsigned int &>(prd.ray_path_index), // Ray path index.
                reinterpret_cast<unsigned int &>(prd.depth)           // Current recursion depth.
            );
        }
    }

    // Store the updated payload.
    setPayload(prd);
}

extern "C" __global__ void __miss__ms()
{
    // No action is taken here.
    // This function simply acts as a terminator for rays that miss all geometry.

    /*
    OptixCSP::PerRayData prd = getPayload();
    const int new_depth = prd.depth + 1;

    if (new_depth < params.max_depth) {
        params.hit_point_buffer[params.max_depth * ray_path_index + new_depth] = make_float4(4.0f);
    }
    */

    // Set the payload values to 0, indicating that the ray missed all geometry.
    optixSetPayload_0(0); // Default value
    optixSetPayload_1(0);
}
