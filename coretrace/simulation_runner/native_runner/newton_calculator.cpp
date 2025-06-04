
#include "newton_calculator.hpp"

NewtonCalculator::NewtonCalculator(surface_ptr surf)
    : SurfaceIntersectionCalculator(),
      tolerance(1e-6),
      max_iters(20)
{
}

int NewtonCalculator::intersect(const double PosLoc[3],
                                const double CosLoc[3],
                                double PosXYZ[3],
                                double CosKLM[3],
                                double DFXYZ[3],
                                double *PathLength)
{
    int sts = 0;
    // TODO: Implement this

    // z0 = this->set_zstart();

    // Run newton's method...

    return sts;
}