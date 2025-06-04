#ifndef SOLTRACE_FLAT_CALCULATOR_H
#define SOLTRACE_FLAT_CALCULATOR_H

#include <cassert>

#include "simulation_data/matvec.hpp"
#include "simulation_data/surface.hpp"
#include "surface_intersection_calculator.hpp"

class FlatCalculator : public SurfaceIntersectionCalculator
{
public:
    FlatCalculator(surface_ptr surf) {}
    virtual ~FlatCalculator() {}
    virtual int intersect(const double PosLoc[3],
                          const double CosLoc[3],
                          double PosXYZ[3],
                          double CosKLM[3],
                          double DFXYZ[3],
                          double *PathLength)
    {

        // TODO: Make sure that intersection should be computed
        // in local coordinate frame.
        double x0 = PosLoc[0];
        double y0 = PosLoc[1];
        double z0 = PosLoc[2];
        double mx = CosLoc[0];
        double my = CosLoc[1];
        double mz = CosLoc[2];
        double t;

        if (fabs(mz) < 1e-12)
        {
            // Ray is parallel to the flat plane.
            // It does not intersect.
            ZeroVec3(PosXYZ);
            ZeroVec3(DFXYZ);
            ZeroVec3(CosKLM);

            *PathLength = -1.0;
        }
        else
        {
            CopyVec3(CosKLM, CosLoc);
            DFXYZ[0] = DFXYZ[1] = 0.0;
            DFXYZ[2] = 1.0;

            t = -z0 / mz;

            PosXYZ[0] = x0 + t * mx;
            PosXYZ[1] = y0 + t * my;
            PosXYZ[2] = 0.0;
            
            assert(fabs(z0 + t * mz) < 1e-12);

            *PathLength = t;
        }

        return 0;
    }
};

#endif
