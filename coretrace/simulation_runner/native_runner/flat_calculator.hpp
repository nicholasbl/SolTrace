#ifndef SOLTRACE_FLAT_CALCULATOR_H
#define SOLTRACE_FLAT_CALCULATOR_H

#include <cassert>

#include "matvec.hpp"
#include "surface.hpp"
#include "surface_intersection_calculator.hpp"

class FlatCalculator : public SurfaceIntersectionCalculator
{
public:
    FlatCalculator(surface_ptr surf);
    virtual ~FlatCalculator();
    virtual int intersect(const double PosLoc[3],
                          const double CosLoc[3],
                          double PosXYZ[3],
                          double CosKLM[3],
                          double DFXYZ[3],
                          double *PathLength);
};

#endif
