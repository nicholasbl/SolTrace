#ifndef SOLTRACE_BBOX_CALCULATOR_H
#define SOLTRACE_BBOX_CALCULATOR_H

// #include "types.h"

#include <native_runner_types.hpp>

namespace SolTrace::EmbreeRunner
{
    enum class BBOXERRORS
    {
        NONE,
        BOUNDS_APERTURE_ERROR,
        BOUNDS_SURFACE_ERROR
    };

    BBOXERRORS get_bounds(SolTrace::NativeRunner::TElement *st_element,
                          float (&min_coord_global)[3],
                          float (&max_coord_global)[3]);

} // namespace embree_helper

#endif
