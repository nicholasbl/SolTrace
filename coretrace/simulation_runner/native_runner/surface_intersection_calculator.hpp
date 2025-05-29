#ifndef SOLTRACE_SURFACE_INTERSECTION_CALCULATOR_H
#define SOLTRACE_SURFACE_INTERSECTION_CALCULATOR_H

#include <memory>

class SurfaceIntersectionCalculator
{
public:
    SurfaceIntersectionCalculator() {}
    ~SurfaceIntersectionCalculator() {}

    virtual int intersect(const double PosLoc[3],
                          const double CosLoc[3],
                          double PosXYZ[3],
                          double CosKLM[3],
                          double DFXYZ[3],
                          double *PathLength) = 0;

private:
};

using calculator_ptr = typename std::shared_ptr<SurfaceIntersectionCalculator>;

#endif
