#pragma once
                
#include <cstdint>

namespace OptixCSP {

    // Hit type
    // TODO: Replace this with RayEvent from simulation_result.hpp?
    enum HitType : uint8_t {
        HIT_UNASSIGNED = 0,
        HIT_CREATE = 1,
        HIT_ABSORB = 2,
        HIT_REFLECT = 3,
        HIT_TRANSMIT = 4,
        HIT_VIRTUAL = 5,
        HIT_EXIT = 6,
        HIT_UNKNOWN = 255
    };

    enum OpticalDistribution : uint8_t {
        OPT_NONE = 0,
        OPT_GAUSSIAN = 1,
        OPT_PILLBOX = 2,
        OPT_UNKNOWN = 255
    };

    enum SunShape :uint8_t {
        SHAPE_NONE = 0,
        SHAPE_GAUSSIAN = 1,
        SHAPE_PILLBOX = 2,
        SHAPE_UNKNOWN = 255
    };

    // Element ID consts
    constexpr int32_t kElementIdBuffer = 0;
    constexpr int32_t kElementIdRayGen = -1;
    constexpr int32_t kElementIdUnassigned = -2;

}