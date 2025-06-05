
#include "newton_calculator.hpp"

#include "vector3d.hpp"

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
    int sts = 1;
    uint_fast64_t count = 0;

    // double x0 = PosLoc[0], y0 = PosLoc[1], z0 = PosLoc[2];
    // double mx = CosLoc[0], my = CosLoc[1], mz = CosLoc[2];
    Vector3d u0(PosLoc);
    Vector3d dt(CosLoc);

    double t0 = 0.0;
    double delta;
    // double res;
    double fvalue;
    double fprime;
    Vector3d v0(PosLoc);
    Vector3d du;

    *PathLength = 0.0;
    ZeroVec3(PosXYZ);
    ZeroVec3(CosKLM);
    ZeroVec3(DFXYZ);

    this->set_zstart(u0.data);
    this->surface_and_jacobian(u0.data, &fvalue, du.data);
    fprime = dot_product(du, dt);

    while (fvalue > this->tolerance && count <= this->max_iters)
    {
        delta = fvalue / fprime;
        t0 -= delta;
        // Updates v0 to (x0, y0, z0) + t0 * (mx, my, mz)
        vector_add(1.0, u0, t0, dt, v0);
        this->surface_and_jacobian(u0.data, &fvalue, du.data);
    }

    if (fvalue <= this->tolerance)
    {
        sts = 0;
        *PathLength = t0;
        CopyVec3(CosKLM, CosLoc);
        CopyVec3(PosXYZ, u0.data);
        CopyVec3(DFXYZ, du.data);
    }

    return sts;
}