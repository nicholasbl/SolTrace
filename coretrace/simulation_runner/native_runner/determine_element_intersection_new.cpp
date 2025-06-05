
// #include "element_intersection.hpp"
#include "determine_element_intersection_new.hpp"

// #include "quadric_calculator.hpp"

// #define sign(x) (x >= 0)
// #define sqr(x) (x * x)

void DetermineElementIntersectionNew(
    TElement *Element,
    double PosRayIn[3],
    double CosRayIn[3],
    double PosRayOut[3],
    double CosRayOut[3],
    double DFXYZ[3],
    double *PathLength,
    int *ErrorFlag,
    int *Intercept,
    int *BacksideFlag)
{
    double x, y;

    *ErrorFlag = 0;

    // find intersection with surface first
    // Intersect(PosRayIn, CosRayIn, Element,
    //           PosRayOut, CosRayOut, DFXYZ, PathLength, ErrorFlag);
    *ErrorFlag = Element->icalc->intersect(PosRayIn, CosRayIn,
                                           PosRayOut, CosRayOut,
                                           DFXYZ, PathLength);
    if (*ErrorFlag > 0 || *PathLength < 0)
    {
        *Intercept = 0;
        PosRayOut[0] = 0.0;
        PosRayOut[1] = 0.0;
        PosRayOut[2] = 0.0;
        CosRayOut[0] = 0.0;
        CosRayOut[1] = 0.0;
        CosRayOut[2] = 0.0;
        DFXYZ[0] = 0.0;
        DFXYZ[1] = 0.0;
        DFXYZ[2] = 0.0;
        *BacksideFlag = 0;
        *PathLength = 0.0;
        return;
    }

    x = PosRayOut[0];
    y = PosRayOut[1];

    if (Element->aperture->is_in(x, y))
    {
        *BacksideFlag = DOT(CosRayIn, DFXYZ) < 0 ? 0 : 1;
        *Intercept = 1;
    }
    else
    {
        *Intercept = 0;
        PosRayOut[0] = 0.0;
        PosRayOut[1] = 0.0;
        PosRayOut[2] = 0.0;
        CosRayOut[0] = 0.0;
        CosRayOut[1] = 0.0;
        CosRayOut[2] = 0.0;
        DFXYZ[0] = 0.0;
        DFXYZ[1] = 0.0;
        DFXYZ[2] = 0.0;
        *PathLength = 0.0;
        *ErrorFlag = 0;
        *BacksideFlag = 0;
    }

    // if hit on backside of element then slope of surface is reversed
    if (*BacksideFlag)
    {
        DFXYZ[0] = -DFXYZ[0];
        DFXYZ[1] = -DFXYZ[1];
        DFXYZ[2] = -DFXYZ[2];
    }

    return;
}

// #undef sign
// #undef sqr
