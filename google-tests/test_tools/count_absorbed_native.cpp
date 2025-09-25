#include "count_absorbed_native.h"

#include <native_runner_types.hpp>

uint_fast64_t count_absorbed_native(const SolTrace::NativeRunner::TRayData *ray_data)
{
    uint_fast64_t count = 0;
    size_t n = ray_data->Count();
    for (size_t i = 0; i < n; i++)
    {
        double pos[3], cos[3];
        int elm, stage;
        unsigned int ray;
        SolTrace::Result::RayEvent rev;
        if (ray_data->Query(i, pos, cos, &elm, &stage, &ray, &rev))
        {
            // if (elm < 0)
            //     ++count;
            // std::cout << "Ray Event: " << ray_event_string(rev) << std::endl;
            if (rev == SolTrace::Result::RayEvent::ABSORB)
                ++count;
        }
    }
    return count;
}
