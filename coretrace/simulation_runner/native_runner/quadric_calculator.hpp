#ifndef SOLTRACE_QUADRIC_CALCULATOR_H
#define SOLTRACE_QUADRIC_CALCULATOR_H

#include "surface_intersection_calculator.hpp"

#include "surface.hpp"

class QuadricCalculator : public SurfaceIntersectionCalculator
{
public:
    QuadricCalculator(surface_ptr surf);
    virtual ~QuadricCalculator();

    virtual int intersect(const double PosLoc[3],
                          const double CosLoc[3],
                          double PosXYZ[3],
                          double CosKLM[3],
                          double DFXYZ[3],
                          double *PathLength);

    SurfaceType get_type() const { return this->my_type; }

    SurfaceType my_type;
    double a2;
    double b2;
    double c2;
    double Kx;
    double Ky;
    double Kz;
    double r;
    double Xc;
    double Yc;
    double Zc;
private:
};

void QuadricSurfaceClosedForm(
    const QuadricCalculator *qc,
    const double PosLoc[3],
    const double CosLoc[3],
    double PosXYZ[3],
    double DFXYZ[3],
    double *PathLength,
    int *ErrorFlag);

#endif
