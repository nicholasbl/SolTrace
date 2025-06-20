#ifndef SOLTRACE_CYLINDER_CALCULATOR_H
#define SOLTRACE_CYLINDER_CALCULATOR_H

#include "surface_intersection_calculator.hpp"

#include "aperture.hpp"
#include "surface.hpp"

class CylinderCalculator : public SurfaceIntersectionCalculator
{
public:
    CylinderCalculator(surface_ptr surf, aperture_ptr ap);
    virtual ~CylinderCalculator();
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
    // double length_x;
    double length_y;
};

#endif
