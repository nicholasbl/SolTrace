
#include "quadric_calculator.hpp"

#include <cassert>
#include <cmath>
#include <limits>

#include "surface.hpp"

#define sqr(x) (x * x)

void QuadricSurfaceClosedForm(
    // TElement *Element,
    const QuadricCalculator *qc,
    const double PosLoc[3],
    const double CosLoc[3],
    double PosXYZ[3],
    double DFXYZ[3],
    double *PathLength,
    int *ErrorFlag)
{
    double Xdelta = 0.0, Ydelta = 0.0, Zdelta = 0.0;
    // double Xc = 0, Yc = 0, Zc = 0, Kx = 0, Ky = 0, Kz = 0;
    // double r = 0.0, r2 = 0.0, a2 = 0, b2 = 0, c2 = 0;
    double r2 = 0.0;
    double t1 = 0.0, t2 = 0.0, A = 0, B = 0, C = 0, slopemag = 0.0;

    double Xc = qc->Xc;
    double Yc = qc->Yc;
    double Zc = qc->Zc;
    double Kx = qc->Kx;
    double Ky = qc->Ky;
    double Kz = qc->Kz;
    double r = qc->r;
    double a2 = qc->a2;
    double b2 = qc->b2;
    double c2 = qc->c2;

    *ErrorFlag = 0;

    // switch (Element->SurfaceIndex)
    switch (qc->get_type())
    {
    // case 's':
    // case 'S': // sphere
    case SPHERE:
        // a2 = 1;
        // b2 = 1;
        // c2 = 1;
        // Kx = 1;
        // if (Element->SurfaceType == 7) // Single-axis curvature aperture
        //     Ky = 0;
        // else
        //     Ky = 1;
        // Kz = 1;
        // r = 1.0 / Element->VertexCurvX;
        r2 = r * r;
        // Xc = 0.0;
        // Yc = 0.0;
        // Zc = r;

        Xdelta = PosLoc[0] - Xc;
        Ydelta = PosLoc[1] - Yc;
        Zdelta = PosLoc[2] - Zc;

        A = CosLoc[0] * CosLoc[0] * Kx / a2 + CosLoc[1] * CosLoc[1] * Ky / b2 + CosLoc[2] * CosLoc[2] * Kz / c2;
        B = 2.0 * (Kx * Xdelta * CosLoc[0] / a2 + Ky * Ydelta * CosLoc[1] / b2 + Kz * Zdelta * CosLoc[2] / c2);
        C = Kx * Xdelta * Xdelta / a2 + Ky * Ydelta * Ydelta / b2 + Kz * Zdelta * Zdelta / c2 - r2;
        break;

    // case 'p':
    // case 'P': // parabola
    case PARABOLA:
        // a2 = 2.0 / Element->VertexCurvX;
        // b2 = Element->VertexCurvY > 0.0 ? 2.0 / Element->VertexCurvY : 1e10;
        // c2 = 1.0;

        // Kx = 1;
        // if (Element->SurfaceType == 7) // Single-axis curvature aperture
        //     Ky = 0;
        // else
        //     Ky = 1;
        // Kz = 0;

        // Xc = 0.0;
        // Yc = 0.0;
        // Zc = 0.0;

        Xdelta = PosLoc[0] - Xc;
        Ydelta = PosLoc[1] - Yc;
        Zdelta = PosLoc[2] - Zc;

        A = CosLoc[0] * CosLoc[0] * Kx / a2 + CosLoc[1] * CosLoc[1] * Ky / b2; // Note A can be zero
        B = 2.0 * (Kx * Xdelta * CosLoc[0] / a2 + Ky * Ydelta * CosLoc[1] / b2) - CosLoc[2] / c2;
        C = Kx * Xdelta * Xdelta / a2 + Ky * Ydelta * Ydelta / b2 - Zdelta / c2;

        break;

    // case 'o':
    // case 'O': // other
    //     a2 = 1;
    //     b2 = 1;
    //     c2 = 1;
    //     Kx = 1;
    //     Ky = 1;
    //     Kz = Element->Kappa;
    //     Xc = 0.0;
    //     Yc = 0.0;
    //     Zc = 1.0 / Element->Kappa / Element->VertexCurvX; // VertexCurvX = VertexCurvY for this surface type

    //     Xdelta = PosLoc[0] - Xc;
    //     Ydelta = PosLoc[1] - Yc;
    //     Zdelta = PosLoc[2] - Zc;

    //     A = CosLoc[0] * CosLoc[0] * Kx / a2 + CosLoc[1] * CosLoc[1] * Ky / b2 + CosLoc[2] * CosLoc[2] * Kz / c2;
    //     B = 2.0 * (Kx * Xdelta * CosLoc[0] / a2 + Ky * Ydelta * CosLoc[1] / b2 + Kz * Zdelta * CosLoc[2] / c2);
    //     C = Kx * Xdelta * Xdelta / a2 + Ky * Ydelta * Ydelta / b2 + Kz * Zdelta * Zdelta / c2 - 1.0 / Element->Kappa / (Element->VertexCurvX * Element->VertexCurvX);
    //     break;

    // case 't':
    // case 'T': // cylinder
    case CYLINDER:
        // a2 = 1;
        // b2 = 1;
        // c2 = 1;
        // Kx = 1;
        // Ky = 0;
        // Kz = 1;
        // r = 1.0 / Element->CurvOfRev;
        // r2 = r * r;
        // Xc = 0.0;
        // Yc = 0.0;
        // Zc = r;

        Xdelta = PosLoc[0] - Xc;
        Ydelta = PosLoc[1] - Yc;
        Zdelta = PosLoc[2] - Zc;

        A = CosLoc[0] * CosLoc[0] * Kx / a2 + CosLoc[1] * CosLoc[1] * Ky / b2 + CosLoc[2] * CosLoc[2] * Kz / c2;
        B = 2.0 * (Kx * Xdelta * CosLoc[0] / a2 + Ky * Ydelta * CosLoc[1] / b2 + Kz * Zdelta * CosLoc[2] / c2);
        C = Kx * Xdelta * Xdelta / a2 + Ky * Ydelta * Ydelta / a2 + Kz * Zdelta * Zdelta / c2 - r2;
        break;
    }

    if (fabs(A) < 1e-12) // Should only happen for parabolas
    {
        t1 = -C / B;
        if (t1 > 0)
        {
            PosXYZ[0] = PosLoc[0] + t1 * CosLoc[0];
            PosXYZ[1] = PosLoc[1] + t1 * CosLoc[1];
            PosXYZ[2] = PosLoc[2] + t1 * CosLoc[2];
            *PathLength = t1;
            // goto Label_100;
        }
        else
        {
            *PathLength = 0.0; // ray tangent or missed
            *ErrorFlag = 1;
            return;
        }
    }
    else if (sqr(B) > 4.0 * A * C)
    {
        t1 = (-B + sqrt(sqr(B) - 4.0 * A * C)) / (2.0 * A);
        t2 = (-B - sqrt(sqr(B) - 4.0 * A * C)) / (2.0 * A);
        if (t2 > 0) // initial ray location outside surface
        {
            PosXYZ[0] = PosLoc[0] + t2 * CosLoc[0];
            PosXYZ[1] = PosLoc[1] + t2 * CosLoc[1];
            PosXYZ[2] = PosLoc[2] + t2 * CosLoc[2];
            *PathLength = t2;

            // TODO: Do we need the below stuff?

            // //*************************************************************************************************************
            // // makes sure to get shortest ray path on valid side of surface; 10-05-10    for open surface of parabola
            // // if cylinder, then PosXYZ[3] will always be less than or equal to Element.Zaperture so never passes this test.
            // // Test for  cylinder follows below.
            // if (PosXYZ[2] > Element->ZAperture)
            // {
            //     PosXYZ[0] = PosLoc[0] + t1 * CosLoc[0];
            //     PosXYZ[1] = PosLoc[1] + t1 * CosLoc[1];
            //     PosXYZ[2] = PosLoc[2] + t1 * CosLoc[2];
            //     *PathLength = t1;
            // }

            // // Remember at this point, intersection is being found on an INFINITELY
            // // long cylinder. If 1st intersection on INFINITELY long cylinder is
            // // from the outside, t2, check to make sure intersection is within the
            // // finite length of the actual cylinder geometry, if not then 2nd
            // // intersection on the inside, t1, is valid one to use.  This
            // // means ray could enter from the open end  of the cylinder and hit
            // // on the inside.  The final test for this is in the calling routine:
            // // DetermineElementIntersectionNew
            // // Wendelin 10-05-10
            // if ((Element->SurfaceIndex == 't') || (Element->SurfaceIndex == 'T'))
            // {
            //     // TODO: Figure out what to do for below when implementing Cylinder
            //     // surface class and SingleAxisCurvature aperture class
            //     // if ((PosXYZ[1] < -Element->ParameterC / 2.0) ||
            //     //     (PosXYZ[1] > Element->ParameterC / 2.0))
            //     // {
            //     //     PosXYZ[0] = PosLoc[0] + t1 * CosLoc[0];
            //     //     PosXYZ[1] = PosLoc[1] + t1 * CosLoc[1];
            //     //     PosXYZ[2] = PosLoc[2] + t1 * CosLoc[2];
            //     //     *PathLength = t1;
            //     // }
            // }

            // // Partial cylinder (sphere with single-axis curvature) needs the
            // // same check as cylinder. Two intersections are possible, check
            // // if the first intersection along the ray path occurs within the
            // // length bounds and, if not, return the second intersection. The
            // // final test for a positive ray path and a valid intersection
            // // location is in DetermineElementIntersectionNew. If t1 is
            // // negative, this intersection location will be ignored in
            // // DetermineElementIntersectionNew
            // if ((Element->SurfaceIndex == 's' || Element->SurfaceIndex == 'S') && (Element->SurfaceType == 7))
            // {
            //     // TODO: Figure out what to do for below when implementing Cylinder
            //     // surface class and SingleAxisCurvature aperture class
            //     // if ((PosXYZ[1] < -Element->ParameterC / 2.0) || (PosXYZ[1] > Element->ParameterC / 2.0))
            //     // {
            //     //     PosXYZ[0] = PosLoc[0] + t1 * CosLoc[0];
            //     //     PosXYZ[1] = PosLoc[1] + t1 * CosLoc[1];
            //     //     PosXYZ[2] = PosLoc[2] + t1 * CosLoc[2];
            //     //     *PathLength = t1;
            //     // }
            // }
            // //***********************************************************************************************************

            // // goto Label_100;
        }
        else if (t2 == 0) // initial ray location at surface
        {
            PosXYZ[0] = PosLoc[0] + t1 * CosLoc[0];
            PosXYZ[1] = PosLoc[1] + t1 * CosLoc[1];
            PosXYZ[2] = PosLoc[2] + t1 * CosLoc[2];
            *PathLength = t1;
            // goto Label_100;
        }
        else if (t2 < 0 && t1 > 0) // initial ray location inside surface
        {
            PosXYZ[0] = PosLoc[0] + t1 * CosLoc[0];
            PosXYZ[1] = PosLoc[1] + t1 * CosLoc[1];
            PosXYZ[2] = PosLoc[2] + t1 * CosLoc[2];
            *PathLength = t1;
            // goto Label_100;
        }
        else if (t1 <= 0)
        {
            *PathLength = t1; // ray heading away from surface
            *ErrorFlag = 1;
            return;
        }
    }
    else
    {
        *PathLength = 0.0; // ray tangent or missed
        *ErrorFlag = 1;
        return;
    }

    if (qc->get_type() == PARABOLA)
    {
        slopemag = sqrt(sqr(2.0 * Kx * (PosXYZ[0] - Xc) / a2) + sqr(2.0 * Ky * (PosXYZ[1] - Yc) / b2) + 1.0);
        DFXYZ[0] = -(2.0 * Kx * (PosXYZ[0] - Xc) / a2) / slopemag;
        DFXYZ[1] = -(2.0 * Ky * (PosXYZ[1] - Yc) / b2) / slopemag;
        DFXYZ[2] = 1.0 / slopemag;
    }
    else
    {
        slopemag = sqrt(sqr(2.0 * Kx * (PosXYZ[0] - Xc) / a2) + sqr(2.0 * Ky * (PosXYZ[1] - Yc) / b2) + sqr(2.0 * Kz * (PosXYZ[2] - Zc) / c2));
        DFXYZ[0] = -(2.0 * Kx * (PosXYZ[0] - Xc) / a2) / slopemag;
        DFXYZ[1] = -(2.0 * Ky * (PosXYZ[1] - Yc) / b2) / slopemag;
        DFXYZ[2] = -(2.0 * Kz * (PosXYZ[2] - Zc) / c2) / slopemag;
    }
}

QuadricCalculator::QuadricCalculator(surface_ptr surf)
{

    // TODO: Redo this without the if else tree...

    if (surf->get_type() == SPHERE)
    {
        auto sphere = std::dynamic_pointer_cast<Sphere>(surf);
        assert(sphere != nullptr);
        a2 = 1;
        b2 = 1;
        c2 = 1;
        Kx = 1;
        // if (Element->SurfaceType == 7) // Single-axis curvature aperture
        //     Ky = 0;
        // else
        //     Ky = 1;
        Kz = 1;
        // r = 1.0 / Element->VertexCurvX;
        r = 1.0 / sphere->vertex_curv;
        // r2 = r * r;
        Xc = 0.0;
        Yc = 0.0;
        Zc = r;
    }
    else if (surf->get_type() == PARABOLA)
    {
        auto para = std::dynamic_pointer_cast<Parabola>(surf);
        assert(para != nullptr);
        a2 = 2.0 / para->vertex_x_curv;
        if (para->vertex_y_curv > 0.0)
            b2 = 2.0 / para->vertex_y_curv;
        else
            b2 = std::numeric_limits<double>::infinity();
        c2 = 1.0;

        Kx = 1;
        // TODO: What is the correct value for Ky here?
        Ky = 1;
        // if (Element->SurfaceType == 7) // Single-axis curvature aperture
        //     Ky = 0;
        // else
        //     Ky = 1;
        Kz = 0;

        Xc = 0.0;
        Yc = 0.0;
        Zc = 0.0;
    }
    else
    {
        // TODO: Need some sort of error here...
    }

    this->my_type = surf->get_type();

    return;
}

QuadricCalculator::~QuadricCalculator() {}

int QuadricCalculator::intersect(const double PosLoc[3],
                                 const double CosLoc[3],
                                 double PosXYZ[3],
                                 double CosKLM[3],
                                 double DFXYZ[3],
                                 double *PathLength)
{
    int sts = 0;
    QuadricSurfaceClosedForm(this, PosLoc, CosLoc, 
        PosXYZ, DFXYZ, PathLength, &sts);
    return sts;
}

#undef sqr
