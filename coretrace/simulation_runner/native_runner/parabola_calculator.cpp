
#include "parabola_calculator.hpp"

#include <cassert>
#include <cmath>
#include <memory>

#include "matvec.hpp"
#include "surface.hpp"
// #include "vector3d.hpp"

ParabolaCalculator::ParabolaCalculator(surface_ptr surf)
{
    auto para = std::dynamic_pointer_cast<Parabola>(surf);

    // TODO: This is an error but should never happen!
    // How to handle it?
    assert(para != nullptr);

    // TODO: Check that this is the correct thing to do
    this->cx = para->vertex_x_curv;
    this->cy = para->vertex_y_curv;

    return;
}

ParabolaCalculator::~ParabolaCalculator()
{
}

int ParabolaCalculator::intersect(const double PosLoc[3],
                                  const double CosLoc[3],
                                  double PosXYZ[3],
                                  double CosKLM[3],
                                  double DFXYZ[3],
                                  double *PathLength)
{
    int sts = 0;

    double x0 = PosLoc[0], y0 = PosLoc[1], z0 = PosLoc[2];
    double mx = CosLoc[0], my = CosLoc[1], mz = CosLoc[2];
    double cx = this->cx, cy = this->cy;
    double t1, t2;
    double a, b, c;

    c = 0.5 * (cx * x0 * x0 + cy * y0 * y0) - z0;
    b = (x0 * cx * mx + y0 * cy * my - mz);
    a = 0.5 * (cx * mx * mx + cy * my * my);

    ZeroVec3(PosXYZ);
    ZeroVec3(CosKLM);
    ZeroVec3(DFXYZ);

    if (fabs(a) < 1e-12)
    {
        // This should only happen if mx == my == 0.0
        t1 = -c / b;
        if (t1 <= 0.0)
        {
            // Intersection is behind the ray -- same as
            // no solution.
            sts = 1;
            *PathLength = 0.0;
        }
        else
        {
            *PathLength = t1;
            SetVec3(PosXYZ, x0 + t1 * mx, y0 + t1 * my, z0 + t1 * mz);
            this->surface_normal(PosXYZ, DFXYZ);
        }
    }
    else
    {
        double scratch = b * b - 4.0 * a * c;
        if (scratch < 0.0)
        {
            // Only imaginary solutions so the ray does
            // NOT intersect the surface
            sts = 1;
            *PathLength = 0.0;
        }
        else
        {
            // TODO: In quadricsurfaceclosedform.cpp checks for
            // PosXYZ[2] > Element->ZAperture. Do we need to do
            // this check here?

            scratch = sqrt(scratch);
            // Negative root
            t1 = -0.5 * (b + scratch) / a;
            // Positive root
            t2 = -0.5 * (b - scratch) / a;
            
            if (t1 > 0.0)
            {
                *PathLength = t1;
                SetVec3(PosXYZ, x0 + t1 * mx, y0 + t1 * my, z0 + t1 * mz);
                this->surface_normal(PosXYZ, DFXYZ);
            }
            else if (t2 > 0.0)
            {
                *PathLength = t2;
                SetVec3(PosXYZ, x0 + t2 * mx, y0 + t2 * my, z0 + t2 * mz);
                this->surface_normal(PosXYZ, DFXYZ);
            }
            else
            {
                sts = 1;
                *PathLength = 0.0;
            }
            this->surface_normal(PosXYZ, DFXYZ);
        }
    }

    CopyVec3(CosKLM, CosLoc);

    return sts;
}

void ParabolaCalculator::surface_normal(const double PosXYZ[3],
                                        double DFXYZ[3])
{
    double cx = this->cx;
    double cy = this->cy;
    DFXYZ[0] = cx * PosXYZ[0];
    DFXYZ[1] = cy * PosXYZ[1];
    DFXYZ[2] = -1.0;
    return;
}
