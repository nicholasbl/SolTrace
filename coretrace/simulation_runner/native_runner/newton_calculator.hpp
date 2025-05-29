#ifndef SOLTRACE_NEWTON_CALCULATOR_H
#define SOLTRACE_NEWTON_CALCULATOR_H

#include "surface_intersection_calculator.hpp"

#include "surface.hpp"

class NewtonCalculator : public SurfaceIntersectionCalculator
{
public:
    NewtonCalculator(surface_ptr surf);
    virtual ~NewtonCalculator() {}

    virtual int intersect(const double PosLoc[3],
                          const double CosLoc[3],
                          double PosXYZ[3],
                          double CosKLM[3],
                          double DFXYZ[3],
                          double *PathLength);

    virtual void set_zstart() = 0;
    virtual void surface_and_jacobian(const double PosXYZ[3],
                                      double *F,
                                      double DFXYZ[3]) = 0;

private:
    double tolerance;
    uint_fast64_t max_iters;
};

#endif
