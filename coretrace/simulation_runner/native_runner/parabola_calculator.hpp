#ifndef SOLTRACE_PARABOLA_CALCULATOR_H
#define SOLTRACE_PARABOLA_CALCULATOR_H

#include "surface_intersection_calculator.hpp"

#include "aperture.hpp"
#include "surface.hpp"

class ParabolaCalculator : public SurfaceIntersectionCalculator
{
public:
    ParabolaCalculator(surface_ptr surf);
    virtual ~ParabolaCalculator();
    virtual int intersect(const double PosLoc[3],
                          const double CosLoc[3],
                          double PosXYZ[3],
                          double CosKLM[3],
                          double DFXYZ[3],
                          double *PathLength);

    void surface_normal(const double PosXYZ[3], double DFXYZ[3]);

    virtual double compute_z_aperture(aperture_ptr ap);

private:
    double cx;
    double cy;
};

#endif
