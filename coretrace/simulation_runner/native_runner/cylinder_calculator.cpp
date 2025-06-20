
#include "cylinder_calculator.hpp"

#include <cassert>
#include <memory>

#include "aperture.hpp"
#include "matvec.hpp"
#include "surface.hpp"

CylinderCalculator::CylinderCalculator(surface_ptr surf, aperture_ptr ap)
{
    auto cylinder = std::dynamic_pointer_cast<Cylinder>(surf);
    // TODO: This is an error but should never happen!
    // How to handle it?
    assert(cylinder != nullptr);

    auto rect = std::dynamic_pointer_cast<Rectangle>(ap);
    // TODO: This is an error but could happen.
    // How to handle it?
    assert(rect != nullptr);

    assert(fabs(2.0 * cylinder->radius - rect->x_length) < 1e-8);

    this->radius = cylinder->radius;
    this->length_y = rect->y_length;
    // this->length_x = rect->x_length;
    // this->length_x = 2.0 * this->radius;

    // std::cout << "radius: " << radius
    // << "  length y: " << length_y
    // << "\naperture: (" << rect->x_coord << ", " << rect->y_coord << ") -- ("
    // << rect->x_length << ", " << rect->y_length << ")"
    // << std::endl;

    return;
}

CylinderCalculator::~CylinderCalculator()
{
}

int CylinderCalculator::intersect(const double PosLoc[3],
                                  const double CosLoc[3],
                                  double PosXYZ[3],
                                  double CosKLM[3],
                                  double DFXYZ[3],
                                  double *PathLength)
{

    // std::cout << "Computing cylinder intersection" << std::endl;

    int sts = 0;

    double x0 = PosLoc[0], y0 = PosLoc[1], z0 = PosLoc[2];
    double mx = CosLoc[0], my = CosLoc[1], mz = CosLoc[2];
    double r = this->radius;
    double yu = 0.5 * this->length_y;
    double yl = -yu;
    double t1, t2;
    double a, b, c;
    double y1, y2;

    // c = x0 * x0 + z0 * z0 - r * r;
    c = x0 * x0 + z0 * z0 - 2.0 * z0 * r;
    // b = 2.0 * (x0 * mx + z0 * mz);
    b = 2.0 * (x0 * mx + (z0 - r) * mz);
    a = mx * mx + mz * mz;

    ZeroVec3(PosXYZ);
    ZeroVec3(CosKLM);
    ZeroVec3(DFXYZ);

    if (fabs(a) < 1e-12)
    {
        // Ray is parallel to the cylinder -- no solution
        sts = 1;
        *PathLength = 0.0;
    }
    else
    {
        double scratch = b * b - 4.0 * a * c;
        if (scratch < 0.0)
        {
            // Only imaginary solutions to quadratic equation -- no solution
            sts = 1;
            *PathLength = 0.0;
        }
        else
        {
            scratch = sqrt(scratch);
            // Negative root
            t1 = -0.5 * (b + scratch) / a;
            // Positive root
            t2 = -0.5 * (b - scratch) / a;

            y1 = y0 + t1 * my;
            y2 = y0 + t2 * my;

            if (t1 > 0.0 && yl <= y1 && y1 <= yu)
            {
                // First intersection point is on the surface of
                // the finite length cylinder
                *PathLength = t1;
                SetVec3(PosXYZ, x0 + t1 * mx, y1, z0 + t1 * mz);
                this->surface_normal(PosXYZ, DFXYZ);
            }
            else if (t2 > 0.0 && yl <= y2 && y2 <= yu)
            {
                // First intersection point is outside of surface of finite
                // length cylinder. Second intersection point is on the
                // surface of finite cylinder.
                *PathLength = t2;
                SetVec3(PosXYZ, x0 + t2 * mx, y2, z0 + t2 * mz);
                this->surface_normal(PosXYZ, DFXYZ);
            }
            else
            {
                // Both intersection points are not on the surface of the
                // finite length cylinder.
                sts = 1;
                *PathLength = 0.0;
            }
        }
    }

    if (sts == 0)
        CopyVec3(CosKLM, CosLoc);

    // std::cout << "Exiting with status: " << sts << std::endl;

    return sts;
}

void CylinderCalculator::surface_normal(const double PosXYZ[3],
                                        double DFXYZ[3])
{
    // TODO: Need to default to returning the surface normal of
    // whatever is the "front". Is that the inside of the parabola
    // (which is used currently) or the outside?
    DFXYZ[0] = 2.0 * PosXYZ[0];
    DFXYZ[1] = 0.0;
    DFXYZ[2] = 2.0 * PosXYZ[2];
    return;
}
