#ifndef SOLTRACE_SPHERE_CALCULATOR_H
#define SOLTRACE_SPHERE_CALCULATOR_H

#include "surface_intersection_calculator.hpp"

#include "aperture.hpp"
#include "surface.hpp"

class SphereCalculator : public SurfaceIntersectionCalculator
{
public:
    SphereCalculator(surface_ptr surf);
    virtual ~SphereCalculator() {}
    virtual int intersect(const double PosLoc[3],
                          const double CosLoc[3],
                          double PosXYZ[3],
                          double CosKLM[3],
                          double DFXYZ[3],
                          double *PathLength);

    void surface_normal(const double PosXYZ[3], double DFXYZ[3]);

    virtual double compute_z_aperture(aperture_ptr ap);

private:
    double radius;
    double curvature;
};

#endif
