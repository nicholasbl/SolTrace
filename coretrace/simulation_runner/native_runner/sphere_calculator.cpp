
#include "sphere_calculator.hpp"

#include <cassert>
#include <cmath>

#include "aperture.hpp"
#include "matvec.hpp"
#include "surface.hpp"

SphereCalculator::SphereCalculator(surface_ptr surf)
{
    auto sph = std::dynamic_pointer_cast<Sphere>(surf);

    // TODO: This is an error but should never happen!
    // How to handle it?
    assert(sph != nullptr);

    // TODO: This is also an error that should be handled
    // but that should probably be caught upstream when
    // creating the surface object.
    assert(sph->vertex_curv > 0.0);

    this->curvature = sph->vertex_curv;
    this->radius = 1.0 / this->curvature;

    return;
}

int SphereCalculator::intersect(const double PosLoc[3],
                                const double CosLoc[3],
                                double PosXYZ[3],
                                double CosKLM[3],
                                double DFXYZ[3],
                                double *PathLength)
{
    int sts = 0;

    double x0 = PosLoc[0], y0 = PosLoc[1], z0 = PosLoc[2];
    double alpha = CosLoc[0], beta = CosLoc[1], gamma = CosLoc[2];
    double r = this->radius;
    double t1, t2;
    double a, b, c;
    double scratch;

    c = x0 * x0 + y0 * y0 + z0 * z0 - 2.0 * r * z0;
    b = 2.0 * (alpha * x0 + beta * y0 + gamma * (z0 - r));
    a = alpha * alpha + beta * beta + gamma * gamma;

    ZeroVec3(PosXYZ);
    ZeroVec3(CosKLM);
    ZeroVec3(DFXYZ);

    scratch = b * b - 4.0 * a * c;

    if (scratch < 0.0)
    {
        // No intersection
        sts = 1;
        *PathLength = 0.0;
    }
    else
    {
        scratch = sqrt(scratch);
        t1 = -0.5 * (b + scratch) / a;
        t2 = 0.5 * (scratch - b) / a;

        double z1 = z0 + gamma * t1;
        double z2 = z0 + gamma * t2;

        if (t1 > 0.0 && z1 <= r)
        {
            SetVec3(PosXYZ, x0 + t1 * alpha, y0 + t1 * beta, z1);
            *PathLength = t1;
        }
        else if (t2 > 0.0 && z2 <= r)
        {
            SetVec3(PosXYZ, x0 + t2 * alpha, y0 + t2 * beta, z2);
            *PathLength = t2;
        }
        else
        {
            sts = 1;
            *PathLength = 0.0;
        }
    }

    if (sts == 0)
    {
        this->surface_normal(PosXYZ, DFXYZ);
        CopyVec3(CosKLM, CosLoc);
    }

    return sts;
}

void SphereCalculator::surface_normal(const double PosXYZ[3],
                                      double DFXYZ[3])
{
    double x0 = PosXYZ[0];
    double y0 = PosXYZ[1];
    double z0 = PosXYZ[2];
    double r = this->radius;

    DFXYZ[0] = -2.0 * x0;
    DFXYZ[1] = -2.0 * y0;
    DFXYZ[2] = -2.0 * (z0 - r);

    return;
}

double SphereCalculator::compute_z_aperture(aperture_ptr ap)
{
    double zmax = this->radius;
    double R = this->radius;
    double r = ap->radius_circumscribed_circle();

    // std::cout << "Radius: " << R
    //           << "\nAperture Radius: " << r
    //           << "\nSQRT: " << sqrt(R * R - r * r) << std::endl;

    if (r < this->radius)
    {
        zmax = R - sqrt(R * R - r * r);
    }

    return zmax;
}
