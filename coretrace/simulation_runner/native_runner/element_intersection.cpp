
#include "element_intersection.hpp"

#define sign(x) (x >= 0)
#define sqr(x) (x * x)

void DetermineElementIntersectionNew(
    TElement *Element,
    double PosRayIn[3],
    double CosRayIn[3],
    double PosRayOut[3],
    double CosRayOut[3],
    double DFXYZ[3],
    double *PathLength,
    int *ErrorFlag,
    int *Intercept,
    int *BacksideFlag)
{
    double x, y;

    *ErrorFlag = 0;

    // find intersection with surface first
    Intersect(PosRayIn, CosRayIn, Element, PosRayOut, CosRayOut, DFXYZ, PathLength, ErrorFlag);
    if (*ErrorFlag > 0 || *PathLength < 0)
    {
        *Intercept = 0;
        PosRayOut[0] = 0.0;
        PosRayOut[1] = 0.0;
        PosRayOut[2] = 0.0;
        CosRayOut[0] = 0.0;
        CosRayOut[1] = 0.0;
        CosRayOut[2] = 0.0;
        DFXYZ[0] = 0.0;
        DFXYZ[1] = 0.0;
        DFXYZ[2] = 0.0;
        *BacksideFlag = 0;
        *PathLength = 0.0;
        return;
    }

    x = PosRayOut[0];
    y = PosRayOut[1];

    if (Element->aperture->is_in(x, y))
    {
        *BacksideFlag = DOT(CosRayIn, DFXYZ) < 0 ? 0 : 1;
        *Intercept = 1;
    }
    else
    {
        *Intercept = 0;
        PosRayOut[0] = 0.0;
        PosRayOut[1] = 0.0;
        PosRayOut[2] = 0.0;
        CosRayOut[0] = 0.0;
        CosRayOut[1] = 0.0;
        CosRayOut[2] = 0.0;
        DFXYZ[0] = 0.0;
        DFXYZ[1] = 0.0;
        DFXYZ[2] = 0.0;
        *PathLength = 0.0;
        *ErrorFlag = 0;
        *BacksideFlag = 0;
    }

    if (*BacksideFlag) // if hit on backside of element then slope of surface is reversed
    {
        DFXYZ[0] = -DFXYZ[0];
        DFXYZ[1] = -DFXYZ[1];
        DFXYZ[2] = -DFXYZ[2];
    }

    return;
}

void Intersect(double PosLoc[3],
               double CosLoc[3],
               TElement *Element,
               double PosXYZ[3],
               double CosKLM[3],
               double DFXYZ[3],
               double *PathLength,
               int *ErrorFlag)
{
    /*{Purpose: To compute intersection point and direction numbers for surface normal
    at intersection point of ray and surface. Path length is also computed.  From Spencer & Murty paper pg. 674
       Input - PosLoc[3] = Initial position of ray in local coordinate system.
               CosLoc[3] = Initial direction cosines of ray in local system.
               Element.SurfaceType = Surface type flag
                             = 1 for rotationally symmetric surfaces
                             = 2 for torics and cylinders
                             = 3 for plane surfaces
                             = 4 for finite element data surface
                             = 5 for VSHOT data surface
                             = 6 for Zernike Monomial description
                             = 7 for single axis curvature surfaces
                             = 8 for rotationally symmetric polynomial description
                             = 9 for      "          "         cubic spline interpolation
                             =10 for torus
               Element.Alpha = Sensitivity coefficients which specify deviation from conic
                       of revolution. For plane p = kx+ly+mz, Alpha[1] = p, Alpha{2..4] = k,l,m
               Element.VertexCurvX = Vertex Curvature of surface
               Element.Kappa = Surface specifier
                     < 0         ==> Hyperboloid
                     = 0         ==> Paraboloid
                     > 0 and < 1 ==> Hemelipsoid of revolution about major axis
                     = 1         ==> Hemisphere
                     > 1         ==> Hemielipsoid of revolution about minor axis
               Element.ConeHalfAngle = Half-angle of cone for cones or revolution or axicons
               Element.CurvOfRev = Curvature of revolution

       Output - PosXYZ[3] = X, Y, Z coordinate of ray/surface intersection
                CosKLM[3] = direction cosines of ray
                DFXYZ[3]  = direction numbers for the surface normal at the
                            intersection point (partial derivatives with respect to
                            X, Y, Z of surface equation).
                PathLength = Path length
                ErrorFlag  = Error flag
                             = 0 for no errors
                             = 1 for Newton-Raphson iteration failed to converge
                             = 2 for interpolation error in SURFACE procedure} */
    int i = 0;
    double S0 = 0.0, S00 = 0.0, S0A = 0.0;
    double X1 = 0.0, x = 0.0, y = 0.0, r = 0.0;
    double Y10 = 0.0, Y1A = 0.0, X10 = 0.0, X1A = 0.0;
    double Y1 = 0.0;
    double SJ = 0.0;
    double SJ1 = 0.0;
    double DFDXYZ = 0.0;
    double FXYZ = 0.0;
    double ZStart = 0.0, ZA = 0.0;
    double ZStartcs = 0.0, PLengthcs = 0.0;
    int EFlagcs = 0;
    double OuterRadius = 0.0, InnerRadius = 0.0, R1 = 0.0, R1A = 0.0, R10 = 0.0, Z1 = 0.0, dzdR1 = 0.0;
    double S0Aperture = 0.0;
    double Ro = 0.0, Ri = 0.0, XL = 0.0;
    bool ZAInterceptInsideAperture = false;
    double Y2 = 0.0, Y3 = 0.0, Y4 = 0.0;
    double FXY = 0.0;
    double PosDum[3] = {0.0, 0.0, 0.0};
    double PosAtZA[3] = {0.0, 0.0, 0.0};
    double PosAtZ0[3] = {0.0, 0.0, 0.0};
    double P1x = 0.0, P1y = 0.0, P2x = 0.0, P2y = 0.0;
    double P3x = 0.0, P3y = 0.0, P4x = 0.0, P4y = 0.0;
    char ApertureShapeIndex = ' ';
    double PosInputToCS = 0.0;
    int in_quad = 0;

    *ErrorFlag = 0;
    for (i = 0; i < 3; i++)
    {
        PosXYZ[i] = PosLoc[i];
        CosKLM[i] = CosLoc[i];
    }

    // Closed form solutions used for closed surfaces (could use Newton-Raphson also,but would have to
    // pick the correct starting point (i.e. the initial point itself) to converge on first intersection
    // chose closed for cylinder
    if (Element->SurfaceType == 2) // cylinder
    {
        QuadricSurfaceClosedForm(Element, PosLoc, CosLoc, PosXYZ, DFXYZ, PathLength, ErrorFlag);
        return;
    }

    // wendelin 5-26-11 chose not use closed form solution for sphere.
    // this solves for a full spheroid, but can build a full spheroid from two hemispheres with iterative solution
    // JM 6/2023: Using closed form solution for sphere with single axis curvature aperture to avoid numerical problems caused by a bad starting point for Newton-Raphson (algorithm needs to start at a location with a defined z-location on the surface)
    if ((Element->SurfaceType == 1 || Element->SurfaceType == 7) && (Element->SurfaceIndex == 's' || Element->SurfaceIndex == 'S')) // sphere or partial cylinder
    {
        QuadricSurfaceClosedForm(Element, PosLoc, CosLoc, PosXYZ, DFXYZ, PathLength, ErrorFlag);
        return;
    }

    // JM 6/2023: Adding closed form solution for hyperboloids and hemi-ellipsoids
    if ((Element->SurfaceType == 1) && (Element->SurfaceIndex == 'o' || Element->SurfaceIndex == 'O')) // hyperboloid or hemiellipsoid
    {
        QuadricSurfaceClosedForm(Element, PosLoc, CosLoc, PosXYZ, DFXYZ, PathLength, ErrorFlag);
        return;
    }

    // JM 6/2023: Adding closed form solution for parabolas
    if ((Element->SurfaceIndex == 'p' || Element->SurfaceIndex == 'P'))
    {
        QuadricSurfaceClosedForm(Element, PosLoc, CosLoc, PosXYZ, DFXYZ, PathLength, ErrorFlag);
        return;
    }

    if (Element->SurfaceType == 10) // torus
    {
        TorusClosedForm(Element, PosLoc, CosLoc, PosXYZ, DFXYZ, PathLength, ErrorFlag);
        return;
    }

    //--------end of closed form solutions-------------
    //  {If not doing closed form solution, proceed to iterative solution}

    // start of new block for determining starting plane for Newton-Raphson   03-11-03

    /*{First, find starting plane.  The correct choice depends on the z-direction of the ray and the original
    position of the ray relative to the element surface.  First step is to find the intersection point
    of ray with  the element aperture plane and determine if it is inside or outside the aperture.
    Next, find z value of surface at x,y coords of original position.
    This determines which side of the surface equation the original position is. Then proceed through conditionals
    to determine the correct starting plane for Newton-Raphson.} */

    if (Element->ZAperture - PosXYZ[2] == 0.0) // numerical fix? 11-16-06 Tim Wendelin
        S0Aperture = 0.0;
    else
        S0Aperture = (Element->ZAperture - PosXYZ[2]) / (CosKLM[2] + 0.00000000001); // numerical fix? tim wendelin 11-20-06

    x = PosXYZ[0] + CosKLM[0] * S0Aperture; // x,y and radial position in aperture plane
    y = PosXYZ[1] + CosKLM[1] * S0Aperture;

    ZAInterceptInsideAperture = Element->aperture->is_in(x, y);

    ZStart = 0.0; // default for all surfacetypes

    if (Element->SurfaceType != 3 && Element->SurfaceType != 4 && Element->SurfaceType != 9)
    {
        SurfaceZatXYPair(PosXYZ, Element, &FXY, ErrorFlag); // find z value of surface at x,y

        if (PosXYZ[2] <= 0.0 && CosKLM[2] > 0.0) // if ray position below z=0 and pointing up then
        {                                        // ZStart should be z=0 plane.
            ZStart = 0.0;
            // goto Label_10;
        }

        else if (PosXYZ[2] <= FXY && CosKLM[2] > 0.0) // if ray position is below surface equation and pointing up
        {                                             // then ZStart should be z=0 plane.
            ZStart = 0.0;
            // goto Label_10;
        }

        else if (PosXYZ[2] <= FXY && CosKLM[2] < 0.0 && PosXYZ[2] > Element->ZAperture && ZAInterceptInsideAperture)
        {                 // if ray position is below surface equation, above the aperture
            ZStart = 0.0; // plane and pointing down
                          // goto Label_10;                                            //and the interception point with aperture plane is inside of
        } // aperture, then ZStart should be z=0 plane.

        else if (PosXYZ[2] <= FXY && CosKLM[2] < 0.0) // if ray position is below surface equation, pointing down
        {                                             // and hits surface below aperture plane then ZStart should be
            ZStart = Element->ZAperture;              // aperture plane.
                                                      // goto Label_10;
        }

        else if (PosXYZ[2] > FXY && CosKLM[2] < 0.0) // if ray position is above surface and pointing in negative z
        {                                            // direction then ZStart should be z=0 plane
            ZStart = 0.0;
            // goto Label_10;
        }

        else if (PosXYZ[2] > FXY && CosKLM[2] > 0.0)
            ZStart = Element->ZAperture; // if ray position is above the surface and
    } // pointing up then ZStart should be

    // // The following calculates ZStart for surfaces described by cubic spline data only.
    // if (Element->SurfaceType == 9)
    // {
    // 	OuterRadius = Element->CubicSplineXData[Element->CubicSplineXData.size() - 1]; // outer,inner radii (or distance from origin if single axis curvature) of data set
    // 	InnerRadius = Element->CubicSplineXData[0];
    // 	ApertureShapeIndex = Element->ShapeIndex;
    // 	ZA = Element->CubicSplineYData[Element->CubicSplineYData.size() - 1]; // z value at aperture plane ZA

    // 	S00 = -PosXYZ[2] / (CosKLM[2] + 0.00000000001); // numerical fix? tim wendelin 11-20-06; //pathlength from original ray point to z=0 plane

    // 	X10 = PosXYZ[0] + CosKLM[0] * S00; // x,y location of intersection point in z=0 plane
    // 	Y10 = PosXYZ[1] + CosKLM[1] * S00;
    // 	R10 = sqrt(X10 * X10 + Y10 * Y10); // radius of intersection point in z=0 plane

    // 	S0A = (ZA - PosXYZ[2]) / (CosKLM[2] + 0.00000000001); // numerical fix? tim wendelin 11-20-06;  //pathlength from original ray point to aperture plane

    // 	X1A = PosXYZ[0] + CosKLM[0] * S0A; // x,y location of intersection point in aperture plane
    // 	Y1A = PosXYZ[1] + CosKLM[1] * S0A;
    // 	R1A = sqrt(X1A * X1A + Y1A * Y1A); // radius of intersection point in aperture plane

    // 	// original location and direction of ray defines starting plane for Newton-Raphson.  This is split into several
    // 	// sections as can be seen in the following.

    // 	// ray at or above aperture plane, ZA, and heading toward Z0
    // 	if (PosXYZ[2] >= ZA && CosKLM[2] < 0.0)
    // 	{
    // 		// move starting point for ray to aperture plane, so intersects at correct point on cylinder below,  03-20-04
    // 		PosAtZA[0] = X1A;
    // 		PosAtZA[1] = Y1A;
    // 		PosAtZA[2] = ZA;

    // 		//{check R or X position depending if rotationally symmetric curvature or single axis curvature}
    // 		if ((((ApertureShapeIndex == 'a') || (ApertureShapeIndex == 'A')) && (R1A > OuterRadius)) || (((ApertureShapeIndex == 'l') || (ApertureShapeIndex == 'L')) && (X1A > OuterRadius)))
    // 		{
    // 			// find intersection with cylinder at outside edge of dataset.  The z value becomes the new ZStart.
    // 			// NewZStartforCubicSplineSurf(OuterRadius*0.999999, PosLoc, CosLoc, ZStartcs, PLengthcs, EFlagcs); //see PosAtZA comment above
    // 			NewZStartforCubicSplineSurf(OuterRadius * 0.999999, PosAtZA, CosLoc, ApertureShapeIndex, &ZStartcs, &PLengthcs, &EFlagcs);
    // 			if (EFlagcs == 0)
    // 			{
    // 				ZStart = ZStartcs;
    // 				// goto Label_10;
    // 			}
    // 			// ray misses virtual cylinder so move on.
    // 			// goto Label_10;
    // 		}

    // 		//{check R or X position depending if rotationally symmetric curvature or single axis curvature}
    // 		else if ((((ApertureShapeIndex == 'a') || (ApertureShapeIndex == 'A')) && (R1A <= OuterRadius)) || (((ApertureShapeIndex == 'l') || (ApertureShapeIndex == 'L')) && (X1A <= OuterRadius)))
    // 		{
    // 			if ((((ApertureShapeIndex == 'a') || (ApertureShapeIndex == 'A')) && (R10 >= OuterRadius)) || (((ApertureShapeIndex == 'l') || (ApertureShapeIndex == 'L')) && (X10 >= OuterRadius)))
    // 			{
    // 				// find intersection with cylinder at outside edge of dataset.  The z value becomes the new ZStart.
    // 				// NewZStartforCubicSplineSurf(OuterRadius*0.999999, PosLoc, CosLoc, ZStartcs, PLengthcs, EFlagcs); //see PosAtZA comment above
    // 				NewZStartforCubicSplineSurf(OuterRadius * 0.999999, PosAtZA, CosLoc, ApertureShapeIndex, &ZStartcs, &PLengthcs, &EFlagcs);
    // 				if (EFlagcs == 0)
    // 				{
    // 					ZStart = ZStartcs;
    // 					// goto Label_10;
    // 				}

    // 				// ray misses virtual cylinder so move on.
    // 				// goto Label_10;
    // 			}

    // 			ZStart = 0.0;
    // 			// goto Label_10;
    // 		}
    // 	}

    // 	// ray at or below Z0 plane and heading toward ZA
    // 	else if (PosXYZ[2] <= 0.0 && CosKLM[2] > 0.0)
    // 	{
    // 		// move starting point for ray to z=0 plane, so intersects at correct point on cylinder below     03/20/04
    // 		PosAtZ0[0] = X10;
    // 		PosAtZ0[1] = Y10;
    // 		PosAtZ0[2] = 0.0;
    // 		// {check R or X position depending if rotationally symmetric curvature or single axis curvature}
    // 		if ((((ApertureShapeIndex == 'a') || (ApertureShapeIndex == 'A')) && (R10 >= OuterRadius)) || (((ApertureShapeIndex == 'l') || (ApertureShapeIndex == 'L')) && (X10 >= OuterRadius)))
    // 		{
    // 			// find intersection with cylinder at outside edge of dataset.  The z value becomes the new ZStart.
    // 			// NewZStartforCubicSplineSurf(OuterRadius*0.999999, PosLoc, CosLoc, ZStartcs, PLengthcs, EFlagcs); //see PosAtZ0 comment above
    // 			NewZStartforCubicSplineSurf(OuterRadius * 0.999999, PosAtZ0, CosLoc, ApertureShapeIndex, &ZStartcs, &PLengthcs, &EFlagcs);
    // 			if (EFlagcs == 0)
    // 			{
    // 				ZStart = ZStartcs;
    // 				// goto Label_10;
    // 			}
    // 			// ray misses virtual cylinder so move on.
    // 			// goto Label_10;
    // 		}

    // 		//{check R or X position depending if rotationally symmetric curvature or single axis curvature}
    // 		else if ((((ApertureShapeIndex == 'a') || (ApertureShapeIndex == 'A')) && ((R10 < OuterRadius) && (R10 > InnerRadius))) || (((ApertureShapeIndex == 'l') || (ApertureShapeIndex == 'L')) && ((X10 < OuterRadius) && (X10 > InnerRadius))))
    // 		{
    // 			ZStart = 0.0;
    // 			// goto Label_10;
    // 		}

    // 		//{check R or X position depending if rotationally symmetric curvature or single axis curvature}
    // 		else if ((((ApertureShapeIndex == 'a') || (ApertureShapeIndex == 'A')) && (R10 <= InnerRadius)) || (((ApertureShapeIndex == 'l') || (ApertureShapeIndex == 'L')) && (X10 <= InnerRadius)))
    // 		{
    // 			// find intersection with cylinder at inside edge of dataset.  The z value becomes the new ZStart.
    // 			// NewZStartforCubicSplineSurf(InnerRadius/0.999999, PosLoc, CosLoc, ZStartcs, PLengthcs, EFlagcs); //see PosAtZ0 comment above
    // 			NewZStartforCubicSplineSurf(InnerRadius / 0.999999, PosAtZ0, CosLoc, ApertureShapeIndex, &ZStartcs, &PLengthcs, &EFlagcs);
    // 			if (EFlagcs == 0)
    // 			{
    // 				ZStart = ZStartcs;
    // 				// goto Label_10;
    // 			}
    // 			// ray misses virtual cylinder so move on.
    // 			// goto Label_10;
    // 		}
    // 	}

    // 	// ray in between ZA and Z0 planes and headed towared Z0
    // 	else if (PosXYZ[2] < ZA && PosXYZ[2] > 0.0 && CosKLM[2] < 0.0)
    // 	{
    // 		R1 = sqrt(PosXYZ[0] * PosXYZ[0] + PosXYZ[1] * PosXYZ[1]); // ray radial position
    // 		//{check R or X position depending if rotationally symmetric curvature or single axis curvature}
    // 		if ((((ApertureShapeIndex == 'a') || (ApertureShapeIndex == 'A')) && (R1 >= OuterRadius)) || (((ApertureShapeIndex == 'l') || (ApertureShapeIndex == 'L')) && (PosXYZ[0] >= OuterRadius))) // ray radial position outside of dataset
    // 		{
    // 			// find intersection with cylinder at outside edge of dataset.  The z value becomes the new ZStart.
    // 			NewZStartforCubicSplineSurf(OuterRadius * 0.999999, PosLoc, CosLoc, ApertureShapeIndex, &ZStartcs, &PLengthcs, &EFlagcs);
    // 			if (EFlagcs == 0)
    // 			{
    // 				ZStart = ZStartcs;
    // 				// goto Label_10;
    // 			}
    // 			// ray misses virtual cylinder so move on.
    // 			// goto Label_10;
    // 		}

    // 		//{check R or X position depending if rotationally symmetric curvature or single axis curvature}
    // 		else if ((((ApertureShapeIndex == 'a') || (ApertureShapeIndex == 'A')) && ((R1 < OuterRadius) && (R1 > InnerRadius))) || (((ApertureShapeIndex == 'l') || (ApertureShapeIndex == 'L')) && ((PosXYZ[0] < OuterRadius) && (PosXYZ[0] > InnerRadius)))) // ray radial position within dataset boundaries
    // 		{																																																													 // find z value at x,y. this determines if point is above or below curve
    // 			if (ApertureShapeIndex == 'a' || ApertureShapeIndex == 'A')
    // 				PosInputToCS = R1;
    // 			else
    // 				PosInputToCS = PosXYZ[0];

    // 			if (!splint(Element->CubicSplineXData,
    // 						Element->CubicSplineYData,
    // 						Element->CubicSplineY2Data,
    // 						Element->CubicSplineXData.size(),
    // 						PosInputToCS, &Z1, &dzdR1))
    // 			{
    // 				*ErrorFlag = 3;
    // 				return;
    // 			}

    // 			if (Z1 < PosXYZ[2]) // ray is above curve
    // 			{
    // 				//	 {check R or X position depending if rotationally symmetric curvature or single axis curvature}
    // 				if ((((ApertureShapeIndex == 'a') || (ApertureShapeIndex == 'A')) && (R10 >= OuterRadius)) || (((ApertureShapeIndex == 'l') || (ApertureShapeIndex == 'L')) && (X10 >= OuterRadius)))
    // 				{
    // 					// find intersection with cylinder at outside edge of dataset.  The z value becomes the new ZStart.
    // 					NewZStartforCubicSplineSurf(OuterRadius * 0.999999, PosLoc, CosLoc, ApertureShapeIndex, &ZStartcs, &PLengthcs, &EFlagcs);
    // 					if (EFlagcs == 0)
    // 					{
    // 						ZStart = ZStartcs;
    // 						// goto Label_10;
    // 					}
    // 					// ray misses virtual cylinder so move on.
    // 					// goto Label_10;
    // 				}
    // 				else
    // 				{
    // 					ZStart = 0.0;
    // 					// goto Label_10;
    // 				}
    // 			}

    // 			else if (Z1 >= PosXYZ[2]) // ray is below curve
    // 			{
    // 				ZStart = PosXYZ[2];
    // 				// ray misses virtual cylinder so move on.
    // 				// goto Label_10;
    // 			}
    // 			else
    // 			{
    // 				// goto Label_10;
    // 			}
    // 		}

    // 		//{check R or X position depending if rotationally symmetric curvature or single axis curvature}
    // 		else if ((((ApertureShapeIndex == 'a') || (ApertureShapeIndex == 'A')) && (R1 <= InnerRadius)) || (((ApertureShapeIndex == 'l') || (ApertureShapeIndex == 'L')) && (PosXYZ[0] <= InnerRadius))) // ray radial position inside of dataset
    // 		{
    // 			// find intersection with cylinder at inside edge of dataset.  The z value becomes the new ZStart.
    // 			NewZStartforCubicSplineSurf(InnerRadius / 0.999999, PosLoc, CosLoc, ApertureShapeIndex, &ZStartcs, &PLengthcs, &EFlagcs);
    // 			if (EFlagcs == 0)
    // 			{
    // 				ZStart = ZStartcs;
    // 				// goto Label_10;
    // 			}

    // 			// ray misses virtual cylinder so move on.
    // 			// goto Label_10;
    // 		}
    // 	}

    // 	// ray in between ZA and Z0 planes and headed toward ZA
    // 	else if (PosXYZ[2] < ZA && PosXYZ[2] > 0.0 && CosKLM[2] > 0.0)
    // 	{
    // 		R1 = sqrt(PosXYZ[0] * PosXYZ[0] + PosXYZ[1] * PosXYZ[1]); // ray radial position
    // 		// {check R or X position depending if rotationally symmetric curvature or single axis curvature}
    // 		if ((((ApertureShapeIndex == 'a') || (ApertureShapeIndex == 'A')) && (R1 >= OuterRadius)) || (((ApertureShapeIndex == 'l') || (ApertureShapeIndex == 'L')) && (PosXYZ[0] >= OuterRadius))) // ray radial position outside of dataset
    // 		{
    // 			// find intersection with cylinder at outside edge of dataset.  The z value becomes the new ZStart.
    // 			NewZStartforCubicSplineSurf(OuterRadius * 0.999999, PosLoc, CosLoc, ApertureShapeIndex, &ZStartcs, &PLengthcs, &EFlagcs);
    // 			if (EFlagcs == 0)
    // 			{
    // 				ZStart = ZStartcs;
    // 				// goto Label_10;
    // 			}
    // 			// ray misses virtual cylinder so move on.
    // 			// goto Label_10;
    // 		}

    // 		//  {check R or X position depending if rotationally symmetric curvature or single axis curvature}
    // 		else if ((((ApertureShapeIndex == 'a') || (ApertureShapeIndex == 'A')) && ((R1 < OuterRadius) && (R1 > InnerRadius))) || (((ApertureShapeIndex == 'l') || (ApertureShapeIndex == 'L')) && ((PosXYZ[0] < OuterRadius) && (PosXYZ[0] > InnerRadius)))) // ray radial position falls within dataset boundaries
    // 		{																																																													 // find z value at x,y. this determines if point is above or below curve

    // 			if (ApertureShapeIndex == 'a' || ApertureShapeIndex == 'A')
    // 				PosInputToCS = R1;
    // 			else
    // 				PosInputToCS = PosXYZ[0];

    // 			if (!splint(Element->CubicSplineXData,
    // 						Element->CubicSplineYData,
    // 						Element->CubicSplineY2Data,
    // 						Element->CubicSplineXData.size(),
    // 						PosInputToCS, &Z1, &dzdR1))
    // 			{
    // 				*ErrorFlag = 3;
    // 				return;
    // 			}

    // 			if (Z1 < PosXYZ[2]) // ray is above curve
    // 			{
    // 				//{check R or X position depending if rotationally symmetric curvature or single axis curvature}
    // 				if ((((ApertureShapeIndex == 'a') || (ApertureShapeIndex == 'A')) && (R1A >= OuterRadius)) || (((ApertureShapeIndex == 'l') || (ApertureShapeIndex == 'L')) && (X1A >= OuterRadius)))
    // 				{
    // 					// find intersection with cylinder at outside edge of dataset.  The z value becomes the new ZStart.
    // 					NewZStartforCubicSplineSurf(OuterRadius * 0.999999, PosLoc, CosLoc, ApertureShapeIndex, &ZStartcs, &PLengthcs, &EFlagcs);
    // 					if (EFlagcs == 0)
    // 					{
    // 						ZStart = ZStartcs;
    // 						// goto Label_10;
    // 					}
    // 					// ray misses virtual cylinder so move on.
    // 					// goto Label_10;
    // 				}
    // 				// goto Label_10;
    // 			}

    // 			else if (Z1 >= PosXYZ[2]) // ray is below curve
    // 			{
    // 				//{check R or X position depending if rotationally symmetric curvature or single axis curvature}
    // 				if ((((ApertureShapeIndex == 'a') || (ApertureShapeIndex == 'A')) && (R10 < OuterRadius)) || (((ApertureShapeIndex == 'l') || (ApertureShapeIndex == 'L')) && (X10 < OuterRadius)))
    // 				{
    // 					ZStart = 0.0;
    // 					// goto Label_10;
    // 				}

    // 				// {check R or X position depending if rotationally symmetric curvature or single axis curvature}
    // 				else if ((((ApertureShapeIndex == 'a') || (ApertureShapeIndex == 'A')) && (R10 >= OuterRadius)) || (((ApertureShapeIndex == 'l') || (ApertureShapeIndex == 'L')) && (X10 >= OuterRadius)))
    // 				{
    // 					PosDum[0] = X10; // back up to intersection with z=0 plane
    // 					PosDum[1] = Y10;
    // 					PosDum[2] = 0.0;
    // 					// find intersection with cylinder at outside edge of dataset.  The z value becomes the new ZStart.
    // 					NewZStartforCubicSplineSurf(OuterRadius * 0.999999, PosDum, CosLoc, ApertureShapeIndex, &ZStartcs, &PLengthcs, &EFlagcs);
    // 					if (EFlagcs == 0)
    // 					{
    // 						ZStart = ZStartcs;
    // 						// goto Label_10;
    // 					}
    // 					// ray misses virtual cylinder so move on.
    // 					// goto Label_10;
    // 				}
    // 			}
    // 		}

    // 		// {check R or X position depending if rotationally symmetric curvature or single axis curvature}
    // 		else if ((((ApertureShapeIndex == 'a') || (ApertureShapeIndex == 'A')) && (R1 <= InnerRadius)) || (((ApertureShapeIndex == 'l') || (ApertureShapeIndex == 'L')) && (PosXYZ[0] <= InnerRadius))) // ray radial position inside of dataset minimum radius
    // 		{
    // 			// find intersection with cylinder at outside edge of dataset.  The z value becomes the new ZStart.
    // 			NewZStartforCubicSplineSurf(OuterRadius * 0.999999, PosLoc, CosLoc, ApertureShapeIndex, &ZStartcs, &PLengthcs, &EFlagcs);
    // 			if (EFlagcs == 0)
    // 			{
    // 				ZStart = ZStartcs;
    // 				// goto Label_10;
    // 			}
    // 			// ray misses virtual cylinder so move on.
    // 			// goto Label_10;
    // 		}
    // 	}
    // }

    // Label_10:
    if (ZStart - PosXYZ[2] == 0.0) // numerical fix? 11-16-06 Tim Wendelin
        S0 = 0.0;
    else
        S0 = (ZStart - PosXYZ[2]) / (CosKLM[2] + 1e-11); // numerical fix? tim wendelin 11-20-06;   //SO is the pathlength from the initial ray position to the Newton-Raphson starting plane

    X1 = PosXYZ[0] + CosKLM[0] * S0; // from this we calculate the x,y position on ZStart starting plane
    Y1 = PosXYZ[1] + CosKLM[1] * S0;

    SJ1 = 0.0;

    // // JM 10/2023: Check upper and lower bounds for S (distance along ray path from (X1,Y1,Zstart)) to restrict step size for cubic spline
    // // This may slow down the solution - previously the iterations would find an intersection point for the spline, but it would be later disgarded because it is out of bounds, now the loop below will reach the iteration limit before failing is there is no intersection
    // double lower_bound = -1e10;
    // double upper_bound = 1e10;
    // if (Element->SurfaceType == 9)
    // {
    // 	double s_to_xmin = (Element->CubicSplineXData[0] - X1) / (CosKLM[0] + 0.00000000001);									 // Distance along ray path from (X1,Y1,Zstart) to smallest x-coordinate on surface
    // 	double s_to_xmax = (Element->CubicSplineXData[Element->CubicSplineXData.size() - 1] - X1) / (CosKLM[0] + 0.00000000001); // Distance along ray path from (X1,Y1,Zstart) to largest x-coordinate on surface
    // 	lower_bound = fmin(s_to_xmin, s_to_xmax);
    // 	upper_bound = fmax(s_to_xmin, s_to_xmax);
    // }

    i = 0;
    // Begin the Newton-Raphson Iteration
    int OKFlag = 0;

    const uint_fast64_t NumIterations = 20;
    const double Epsilon = 1e-6;

    while (i++ < NumIterations)
    {
        SJ = SJ1;
        PosXYZ[0] = X1 + CosKLM[0] * SJ;
        PosXYZ[1] = Y1 + CosKLM[1] * SJ;
        PosXYZ[2] = ZStart + CosKLM[2] * SJ;

        Surface(PosXYZ, Element, &FXYZ, DFXYZ, &OKFlag);

        // if (OKFlag == 0) goto Label_40;
        if (OKFlag != 0)
        {
            *ErrorFlag = 2; // Interpolation error in Surface procedure
            break;
            // goto Label_100;
        }

        // Label_40:
        DFDXYZ = DOT(DFXYZ, CosKLM);
        if (fabs(FXYZ) <= Epsilon * fabs(DFDXYZ))
        {
            break;
            // goto Label_100;
        }

        SJ1 = SJ - FXYZ / DFDXYZ;

        // // JM 10/2023: Enforce bounds to restrict next guess for cubic spline
        // if (Element->SurfaceType == 9)
        // {
        // 	if ((FXYZ < 0 && CosKLM[2] > 0) || (FXYZ > 0 && CosKLM[2] < 0)) // FXYZ < 0 if current point is below surface, FXYZ > 0 if current point is above surface
        // 		lower_bound = fmax(SJ, lower_bound);
        // 	else if ((FXYZ < 0 && CosKLM[2] < 0) || (FXYZ > 0 && CosKLM[2] > 0))
        // 		upper_bound = fmin(SJ, upper_bound);
        // 	if (SJ1 < lower_bound || SJ1 > upper_bound)
        // 		SJ1 = 0.5 * (lower_bound + upper_bound);
        // 	if (upper_bound <= lower_bound)
        // 	{
        // 		*ErrorFlag = 1; // Failed to converge
        // 		break;
        // 	}
        // }
    }
    if (i == NumIterations)
        *ErrorFlag = 1; // Failed to converge

    // Label_100:
    *PathLength = S0 + SJ;

    return;
}

void Surface(
    double PosXYZ[3],
    TElement *Element,
    double *FXYZ,
    double DFXYZ[3],
    int *ErrorFlag)
{
    /*{Purpose: To compute the surface equation and it's derivatives for various
    geometric surfaces.
        Input - PosXYZ[3] = X, Y, Z coordinate position
                Element.SurfaceType = Surface type flag
                              = 1 for rotationally symmetric surfaces
                              = 2 for torics and cylinders
                              = 3 for plane surfaces
                              = 4 for surface interpolated from finite element data points
                              = 5 for surface interpolated from VSHOT data points
                              = 6 for surface described by Zernike monomials
                              = 7 for single axis parabolic curvature surfaces
                              = 8 for rotationally symmetric polynomial description
                              = 9 for       "          "     cubic spline interpolation
                Element.Alpha = Sensitivity coefficients which specify deviation from conic
                        of revolution
                Element.VertexCurvX = Vertex Curvature of surface
                Element.Kappa = Surface specifier
                     < 0         ==> Hyperboloid
                     = 0         ==> Paraboloid
                     > 0 and < 1 ==> Hemelipsoid of revolution about major axis
                     = 1         ==> Hemisphere
                     > 1         ==> Hemelipsoid of revolution about minor axis
                Element.ConeHalfAngle = Half-angle of cone for cones or revolution or axicons
                Element.CurvOfRev = Curvature of revolution

        Output - FXYZ = Surface equation
                 DFXYZ[3] = Derivatives of surface equation
                 ErrorFlag = Error Flag
                             = 0  ==> no errors
                             > 0  ==> interpolation error
    }*/

    // int i=0,k=0;
    int i = 0;
    double X = 0.0, Y = 0.0, Z = 0.0;
    // double Rho=0.0, Rho2=0.0, Rho2i=0.0;
    double Rho = 0.0, Rho2 = 0.0;
    double DFDX = 0, DFDY = 0, DFDZ = 0;
    double Sum1 = 0.0, Sum2 = 0.0, Term = 0.0;
    double Y2 = 0.0, Y2J = 0.0;
    double FY = 0.0, ZZ = 0.0;
    double zm = 0.0, zr = 0.0;
    double dzrdx = 0.0, dzrdy = 0.0, delzx = 0.0, delzy = 0.0;
    double density = 0.0, delta = 0.0;
    double zx = 0.0, zy = 0.0;
    double dzdRho = 0.0, dRhodx = 0.0, dRhody = 0.0;

    // label 100, 130,160, 850, 990;

    // Initialize variables
    X = PosXYZ[0];
    Y = PosXYZ[1];
    Z = PosXYZ[2];
    *ErrorFlag = 0;

    //===SurfaceType = 1, 7  Rotationally Symmetric surfaces and single axis curvature sections===========================
    if (Element->SurfaceType == 1 || Element->SurfaceType == 7)
    {
        if (Element->SurfaceType == 1)
            Rho2 = X * X + Y * Y; // rotationally symmetric
        else
            Rho2 = X * X; // single axis curvature depends only on x

        if (Element->ConeHalfAngle != 0.0)
            goto Label_160;

        // wendelin 5-18-11 changes to allow different vertex curvature in the x and y directions for the parabola; this block of code
        // is a subset of the more general form below therefore it has been commented out.  It also assumes VertexCurvY = either VertexCurvX or zero
        // and doesn't allow different nonzero values for the parabolic case.   Not using the alpha parameters for the general case for now.
        /*
            for (i=0;i<5;i++)
              if (Element->Alpha[i] != 0.0)
                goto Label_130;

            *FXYZ = Z - 0.5*Element->VertexCurvX*(Rho2 + Element->Kappa*Z*Z);
            DFDX = -Element->VertexCurvX*X;
            DFDY = -Element->VertexCurvY*Y; //VertexCurvY = VertexCurvX if rotationally symmetric or 0 if single axis curved
            DFDZ = 1.0 - Element->Kappa*Element->VertexCurvX*Z;
            goto Label_990;
        */
        Sum1 = 0.0;
        Sum2 = 0.0;
        // Rho2i = 1.0;

        // wendelin 5-18-11
        /*
        for (i=0;i<5;i++)
        {
          Sum1 = i*Element->Alpha[i]*Rho2i + Sum1;
          Rho2i = Rho2i*Rho2;
          Sum2 = Element->Alpha[i]*Rho2i + Sum2;
        }*/

        // wendelin 5-18-11 changes to allow different vertex curvature in the x and y directions for the parabola only
        //  Term = sqrt(1.0 - Element->Kappa*Element->VertexCurvX*Element->VertexCurvX*Rho2);

        if (1.0 - Element->Kappa * (Element->VertexCurvX * Element->VertexCurvX * X * X + Element->VertexCurvY * Element->VertexCurvY * Y * Y) < 0)
        {
            *ErrorFlag = 1; // Surface is not defined at this x,y location
        }

        Term = sqrt(1.0 - Element->Kappa * (Element->VertexCurvX * Element->VertexCurvX * X * X + Element->VertexCurvY * Element->VertexCurvY * Y * Y)); // new
        //*FXYZ = Z - Element->VertexCurvX*Rho2/(1.0 + Term) - Sum2;
        *FXYZ = Z - (Element->VertexCurvX * X * X + Element->VertexCurvY * Y * Y) / (1.0 + Term) - Sum2; // new

        DFDX = -X * (Element->VertexCurvX / Term + 2.0 * Sum1);
        DFDY = -Y * (Element->VertexCurvY / Term + 2.0 * Sum1); // VertexCurvY = VertexCurvX if rotationally symmetric or 0 if single axis curved
        DFDZ = 1.0;
        goto Label_990;

    Label_160:
        *FXYZ = Z - sqrt(Rho2) / tan(Element->ConeHalfAngle * (ACOSM1O180));
        DFDX = -X / (sqrt(Rho2) * tan(Element->ConeHalfAngle * (ACOSM1O180)));
        DFDY = -Y / (sqrt(Rho2) * tan(Element->ConeHalfAngle * (ACOSM1O180)));
        DFDZ = 1.0;
        goto Label_990;
    }

    //===SurfaceType = 2, Toroidal or Cylindrical surfaces========================== //not currently used
    if (Element->SurfaceType == 2)
    {
        Sum1 = 0.0;
        Sum2 = 0.0;
        Y2 = Y * Y;
        Y2J = 1.0;

        for (i = 0; i < 5; i++)
        {
            Sum1 = i * Element->Alpha[i] * Y2J * Y + Sum1;
            Y2J = Y2J * Y2;
            Sum2 = Element->Alpha[i] * Y2J + Sum2;
        }

        Term = sqrt(1.0 - Element->Kappa * Element->VertexCurvX * Element->VertexCurvX * Y2);
        FY = Element->VertexCurvX * Y2 / (1.0 + Term) + Sum2;
        *FXYZ = Z - FY - 0.5 * Element->CurvOfRev * (X * X + Z * Z - FY * FY);
        DFDX = -Element->CurvOfRev * X;
        DFDY = (Element->CurvOfRev * FY - 1.0) * (Element->VertexCurvX * Y / Term + 2.0 * Sum1);
        DFDZ = 1.0 - Element->CurvOfRev * Z;
        goto Label_990;
    }

    //===SurfaceType = 3, Plane Surfaces============================================
    /*     {The equation of a plane is: kx + ly + mz = p,  where k,l,m are the direction
         cosines of the normal to the plane, and p is the distance from the origin
         to the plane.  In this case, these parameters are contained in the Alpha array.}*/
    if (Element->SurfaceType == 3)
    {
        DFDX = Element->Alpha[0];
        DFDY = Element->Alpha[1];
        DFDZ = Element->Alpha[2];
        *FXYZ = DFDX * X + DFDY * Y + DFDZ * Z - Element->Alpha[3];
        goto Label_990;
    }

    // //===SurfaceType = 4, Surface specified by finite element data==================
    //   if (Element->SurfaceType == 4)
    //   {
    //     Rho2 = X*X + Y*Y;
    //     if (Rho2 == 0.0)
    //     {
    //       //FXYZ := Z - ZA[1];  ZA not defined yet
    //       *FXYZ = Z;
    //       DFDX = 0.0;
    //       DFDY = 0.0;
    //       DFDZ = 1;
    //       goto Label_990;
    //     }

    //     //Interpolate to find the z
    //     density = Element->FEData.nodes.size()/Element->ApertureArea;
    //     delta = 0.001/sqrt(density);
    //     FEInterpKD(X, Y, &Element->FEData, delta, &zr, &dzrdx, &dzrdy);

    //     PosXYZ[2] = zr;
    //     *FXYZ = Z - zr;
    //     DFDX = dzrdx;
    //     DFDY = dzrdy;
    //     DFDZ = 1.0;
    //     goto Label_990;
    //   }

    // //===SurfaceType = 5, VSHOT data================================================
    //   if (Element->SurfaceType == 5)
    //   {
    //     Rho2 = X*X + Y*Y;
    //     if (Rho2 == 0.0)
    //     {
    //       *FXYZ = Z;
    //       DFDX = 0.0;
    //       DFDY = 0.0;
    //       DFDZ = 1.0;
    //       goto Label_990;
    //     }
    //     // evaluate z, dz/dx and dz/dy from the monomial fit at x,y
    //     EvalMono(X, Y, Element->BCoefficients, Element->FitOrder, 0.0, 0.0, &zm); //the 0.0's are values for DeltaX and DeltaY; **[need to look at this further]**

    //     //Interpolate to find the slope residuals
    //     density = Element->VSHOTData.nrows()/Element->ApertureArea;

    //     /*
    //     if (Element->ShapeIndex == 'l' || Element->ShapeIndex == 'L')       //interpolation scheme for single axis curvature surfaces
    //       VSHOTInterpolateNew(X, Y, density, Element->VSHOTData, Element->VSHOTData.nrows(), &delzx, &delzy);
    //     else
    //       VSHOTInterpolate(X, Y, density, Element->VSHOTData, Element->VSHOTData.nrows(), &delzx, &delzy);
    //     */

    //     ::VSHOTInterpolateModShepard(X, Y, density, Element->VSHOTData, Element->VSHOTData.nrows(), &delzx, &delzy, ErrorFlag);

    //     if ( *ErrorFlag != 0 ) return;

    //     //Evaluate "real" z (i.e. the best estimate for z comes from the monomial fit)
    //     zr = zm;

    //     //Now evaluate the slopes  -  what we want here is the measured slope which is the best value to use
    //     //dzrdx := dzmdx + delzx;     //fit slope + (fit slope - meas. slope) =  wrong value
    //     //dzrdy := dzmdy + delzy;

    //     //dzrdx := dzmdx - delzx;       //fit slope - (fit slope - meas. slope) = meas. slope  (this is what we want)
    //     //dzrdy := dzmdy - delzy;
    //     dzrdx = delzx;                //if VSHOTInterpolate returns interpolated measured slopes and not slope RESIDUALS
    //     dzrdy = delzy;                // These values are angles of the slope in radians. Need to convert to dimensionless dz/dy and dz/dx so take tangent of angle

    //     dzrdx = tan(dzrdx);
    //     dzrdy = tan(dzrdy);

    //     PosXYZ[2] = zr;
    //     *FXYZ = Z - zr;
    //     DFDX = dzrdx;
    //     DFDY = dzrdy;
    //     //change sign of derivatives to agree with SurfaceType = 1
    //     DFDX = -DFDX;
    //     DFDY = -DFDY;
    //     DFDZ = 1.0;
    //     goto Label_990;
    //   }

    // //===SurfaceType = 6, Zernike monomials=========================================
    //   if (Element->SurfaceType == 6)
    //   {
    //     // k = Element->FitOrder + 1;
    //     ZZ = 0.0;
    //     DFDX = 0.0;
    //     DFDY = 0.0;

    //     // evaluate z from the monomial expression at x,y
    //     EvalMono(X, Y, Element->BCoefficients, Element->FitOrder, 0.0, 0.0, &ZZ); //the 0.0's are values for DeltaX and DeltaY; **[need to look at this further]**
    //     MonoSlope(Element->BCoefficients, Element->FitOrder, X, Y, &DFDX, &DFDY);

    //     PosXYZ[2] = ZZ;
    //     *FXYZ = Z - ZZ;
    //     //{change sign of derivatives to agree with SurfaceType = 1}
    //     DFDX = -DFDX;
    //     DFDY = -DFDY;
    //     DFDZ = 1.0;
    //     goto Label_990;
    //   }

    // //===SurfaceType = 8, rotationally symmetric polynomial surface=============================
    // if (Element->SurfaceType == 8)
    // {
    // 	ZZ = 0.0;
    // 	DFDX = 0.0;
    // 	DFDY = 0.0;

    // 	double yval = Y;
    // 	if (Element->ShapeIndex == 'l' || Element->ShapeIndex == 'L')
    // 		yval = 0.0;

    // 	// evaluate z & slopes from the polynomial expression at r = sqrt(x^2+y^2)
    // 	EvalPoly(X, yval, Element->PolyCoeffs, Element->FitOrder, &ZZ);
    // 	PolySlope(Element->PolyCoeffs, Element->FitOrder, X, yval, &DFDX, &DFDY);

    // 	PosXYZ[2] = ZZ;
    // 	*FXYZ = Z - ZZ;
    // 	//{change sign of derivatives to agree with SurfaceType = 1}
    // 	DFDX = -DFDX;
    // 	DFDY = -DFDY;
    // 	DFDZ = 1.0;
    // 	goto Label_990;
    // }

    // //===SurfaceType = 9, rotationally symmetric cubic spline interpolation surface==============
    // if (Element->SurfaceType == 9)
    // {
    // 	ZZ = 0.0;
    // 	DFDX = 0.0;
    // 	DFDY = 0.0;

    // 	Rho = sqrt(X * X + Y * Y);
    // 	dRhodx = X / Rho;
    // 	dRhody = Y / Rho;

    // 	if (Element->ShapeIndex == 'l' || Element->ShapeIndex == 'L') // x dimension only for single axis curvature
    // 	{
    // 		Rho = X;
    // 		dRhodx = 1.0;
    // 		dRhody = 0.0;
    // 	}

    // 	// evaluate z & slopes using cubic spline interpolation
    // 	if (!splint(Element->CubicSplineXData,
    // 				Element->CubicSplineYData,
    // 				Element->CubicSplineY2Data,
    // 				Element->CubicSplineXData.size(),
    // 				Rho, &ZZ, &dzdRho))
    // 	{
    // 		*ErrorFlag = 3;
    // 		return;
    // 	}

    // 	DFDX = dzdRho * dRhodx;
    // 	DFDY = dzdRho * dRhody;

    // 	PosXYZ[2] = ZZ;
    // 	*FXYZ = Z - ZZ;
    // 	//{change sign of derivatives to agree with SurfaceType = 1}
    // 	DFDX = -DFDX;
    // 	DFDY = -DFDY;
    // 	DFDZ = 1.0;
    // 	goto Label_990;
    // }

    // the following surfacetype is now handled above in the general case

    //===SurfaceType = 7, single axis curvature parabolic or spherical surface=============================
    /*{if SurfaceType = 7 then
    begin
      if (SurfaceIndex = 'p') or (SurfaceIndex = 'P') then
      begin
         FXYZ := Z - X*X*VertexCurvX/2.0;
         DFDX := -X*VertexCurvX;
         DFDY := 0.0;
         DFDZ := 1.0;
      end;
      if (SurfaceIndex = 's') or (SurfaceIndex = 'S') then
      begin
       FXYZ := Z - 0.5*VertexCurvX*(X*X + Z*Z);
       DFDX := -VertexCurvX*X;
       DFDY := 0.0;
       DFDZ := 1.0 - VertexCurvX*Z;
      end;
    end;}*/

//{****Offload derivatives of the surface equation}
Label_990:
    DFXYZ[0] = DFDX;
    DFXYZ[1] = DFDY;
    DFXYZ[2] = DFDZ;
}
// end of procedure--------------------------------------------------------------

void SurfaceZatXYPair(
	double PosXYZ[3],
	TElement *Element,
	double *FXYZ,
	int *ErrorFlag)
{
	/*{Purpose: To compute the Z value of the surface equation at an X,Y pair.
		Input - PosXYZ[3] = X, Y, Z coordinate position
				Element.SurfaceType = Surface type flag
							  = 1 for rotationally symmetric surfaces
							  = 2 for torics and cylinders
							  = 3 for plane surfaces
							  = 4 for surface interpolated from finite element data points
							  = 5 for surface interpolated from VSHOT data points
							  = 6 for surface described by Zernike monomials
							  = 7 for single axis parabolic curvature surfaces
							  = 8 for rotationally symmetric polynomial description
							  = 9 for       "          "     cubic spline interpolation
				Element.Alpha = Sensitivity coefficients which specify deviation from conic
						of revolution
				Element.VertexCurvX = Vertex Curvature of surface
				Element.Kappa = Surface specifier
					 < 0         ==> Hyperboloid
					 = 0         ==> Paraboloid
					 > 0 and < 1 ==> Hemelipsoid of revolution about major axis
					 = 1         ==> Hemisphere
					 > 1         ==> Hemelipsoid of revolution about minor axis
				Element.ConeHalfAngle = Half-angle of cone for cones or revolution or axicons
				Element.CurvOfRev = Curvature of revolution

		Output - FXYZ = Z value of Surface equation
				 ErrorFlag = Error Flag
							 = 0  ==> no errors
							 > 0  ==> interpolation error
	}*/
	int i = 0;
	// double X=0.0,Y=0.0,Z=0.0;
	double X = 0.0, Y = 0.0;
	double Rho2 = 0.0, Rho = 0.0;
	double Sum1 = 0.0, ZZ = 0.0, zm = 0.0;

	// Initialize variables
	X = PosXYZ[0];
	Y = PosXYZ[1];
	// Z = PosXYZ[2];
	*ErrorFlag = 0;

	//===SurfaceType = 1, 7  Rotationally Symmetric surfaces and single axis curvature sections===========================
	if (Element->SurfaceType == 1 || Element->SurfaceType == 7)
	{
		if (Element->SurfaceType == 1)
			Rho2 = X * X + Y * Y; // rotationally symmetric
		else
			Rho2 = X * X; // single axis curvature depends only on x

		Rho = sqrt(Rho2);

		if (Element->ConeHalfAngle != 0.0)
			goto Label_160;

		// wendelin 5-18-11

		// if (Element->Kappa*Element->VertexCurvX*Element->VertexCurvX*Rho2 > 1.0)  //xy pair cannot be found on closed surface   06-10-07
		if (Element->Kappa * (Element->VertexCurvX * Element->VertexCurvX * X * X + Element->VertexCurvY * Element->VertexCurvY * Y * Y) > 1.0) // xy pair cannot be found on closed surface   06-10-07
		{
			*FXYZ = 0.0;
			return;
		}

		// wendelin 5-18-11
		//  *FXYZ = Element->VertexCurvX*Rho2/(1.0+sqrt(1.0-Element->Kappa*Element->VertexCurvX*Element->VertexCurvX*Rho2));
		*FXYZ = (Element->VertexCurvX * X * X + Element->VertexCurvY * Y * Y) / (1.0 + sqrt(1.0 - Element->Kappa * (Element->VertexCurvX * Element->VertexCurvX * X * X + Element->VertexCurvY * Element->VertexCurvY * Y * Y)));

		/*        for (i=0;i<5;i++)
					 if (Element->Alpha[i] != 0.0) goto Label_130;
			   */

		return;

		Sum1 = 0.0;
		for (i = 0; i < 5; i++)
			Sum1 = Element->Alpha[i] * pow(Rho, 2 * (i + 1)) + Sum1;

		*FXYZ += Sum1;
		return;

	Label_160:
		*FXYZ = sqrt(Rho2) / tan(Element->ConeHalfAngle * (ACOSM1O180));
		return;
	}

	//===SurfaceType = 3, Plane Surfaces============================================
	/* {The equation of a plane is: kx + ly + mz = p,  where k,l,m are the direction
	 cosines of the normal to the plane, and p is the distance from the origin
	 to the plane.  In this case, these parameters are contained in the Alpha array.}
	 {if SurfaceType = 3 then
	 begin
		DFDX = Alpha[1];
		DFDY = Alpha[2];
		DFDZ = Alpha[3];
		FXYZ = DFDX*X + DFDY*Y + DFDZ*Z - Alpha[4];
		return;
	 end;}*/

	//===SurfaceType = 4, Surface specified by finite element data==================
	/*{if SurfaceType = 4 then
	 begin
		Rho2 = X*X + Y*Y;
		if Rho2 = 0.0 then
		begin
			 //FXYZ = Z - ZA[1];  ZA not defined yet
			 FXYZ = Z;
			 DFDX = 0.0;
			 DFDY = 0.0;
			 DFDZ = 1;
			 return;
		end;
		  //Interpolate to find the z
		  Density = FENumPoints/ApertureArea;
		  delta = 0.1/sqrt(density);
		  FEInterpNew(X, Y, Density, FEData, FENumPoints, zr);

		  //Now evaluate the slopes
		  FEInterpNew(X+delta, Y, Density, FEData, FENumPoints, zx);
		  FEInterpNew(X, Y+delta, Density, FEData, FENumPoints, zy);
		  dzrdx = (zx-zr)/delta;
		  dzrdy = (zy-zr)/delta;

		  PosXYZ[3] = zr;
		  FXYZ = z - zr;
		  DFDX = dzrdx;
		  DFDY = dzrdy;
		  //change sign of derivatives to agree with SurfaceType = 1
		  DFDX = -DFDX;
		  DFDY = -DFDY;
		  DFDZ = 1.0;
		  return;
	 end;}*/

	// //===SurfaceType = 5, VSHOT data================================================
	//   if (Element->SurfaceType == 5)
	//   {
	//     Rho2 = X*X + Y*Y;
	//     if (Rho2 == 0.0)
	//     {
	//       *FXYZ = 0.0;
	//       return;
	//     }
	//     // evaluate z, dz/dx and dz/dy from the monomial fit at x,y
	//     EvalMono(X, Y, Element->BCoefficients, Element->FitOrder, 0.0, 0.0, &zm); //the 0.0's are values for DeltaX and DeltaY; **[need to look at this further]**
	//     *FXYZ = zm;
	//     return;
	//   }

	// //===SurfaceType = 6, Zernike monomials=========================================
	//   if (Element->SurfaceType == 6)
	//   {
	//           // evaluate z from the monomial expression at x,y
	//     EvalMono(X, Y, Element->BCoefficients, Element->FitOrder, 0.0, 0.0, &ZZ); //the 0.0's are values for DeltaX and DeltaY; **[need to look at this further]**
	//     *FXYZ = ZZ;
	//     return;
	//   }

	// //===SurfaceType = 8, rotationally symmetric polynomial surface=============================
	//   if (Element->SurfaceType == 8)
	//   {
	//     // evaluate z & slopes from the polynomial expression at r = sqrt(x^2+y^2)

	//     double yval = Y;
	//     if ( Element->ShapeIndex == 'l' || Element->ShapeIndex == 'L' )
	//       yval = 0.0;

	//     EvalPoly(X, yval, Element->PolyCoeffs, Element->FitOrder, &ZZ);
	//     *FXYZ = ZZ;
	//     return;
	//   }
	//===SurfaceType = 9, rotationally symmetric cubic spline interpolation surface==============
	/*if (Element->SurfaceType == 9)
  {
   ZZ = 0.0;
   DFDX = 0.0;
   DFDY = 0.0;

   Rho = sqrt(X*X+Y*Y);
   dRhodx = X/Rho;
   dRhody = Y/Rho;
   //evaluate z & slopes using cubic spline interpolation
   splint(Element->CubicSplineXData,
	 Element->CubicSplineYData,
	 Element->CubicSplineY2Data,
	 Element->CubicSplineXData.length(),
	 Rho,
	 &ZZ,&dzdRho);

   DFDX = dzdRho*dRhodx;
   DFDY = dzdRho*dRhody;

   PosXYZ[2] = ZZ;
   *FXYZ = Z - ZZ;
   //change sign of derivatives to agree with SurfaceType = 1
   DFDX = -DFDX;
   DFDY = -DFDY;
   return;
  }*/

	// the following surfacetype is now handled above in the general case

	//===SurfaceType = 7, single axis curvature parabolic or spherical surface=============================
	/*{if SurfaceType = 7 then
	begin
	  if (SurfaceIndex = 'p') or (SurfaceIndex = 'P') then
	  begin
		 FXYZ = Z - X*X*VertexCurvX/2.0;
		 DFDX = -X*VertexCurvX;
		 DFDY = 0.0;
		 DFDZ = 1.0;
	  end;
	  if (SurfaceIndex = 's') or (SurfaceIndex = 'S') then
	  begin
	   FXYZ = Z - 0.5*VertexCurvX*(X*X + Z*Z);
	   DFDX = -VertexCurvX*X;
	   DFDY = 0.0;
	   DFDZ = 1.0 - VertexCurvX*Z;
	  end;
	end;}*/
}

void TorusClosedForm(
    TElement *Element,
    double PosLoc[3],
    double CosLoc[3],
    double PosXYZ[3],
    double DFXYZ[3],
    double *PathLength,
    int *ErrorFlag)
{
    double Xo = 0.0, Yo = 0.0, Zo = 0.0, Epsilon = 0.0, Eta = 0.0;
    double Rho = 0.0, Rs = 0.0, Ra = 0.0, X = 0.0, Y = 0.0, Z = 0.0;
    double Fx = 0.0, Fy = 0.0, Fz = 0.0;
    double amatrix[5][5];
    double rvector[5];
    double imagroot1 = 0.0, imagroot2 = 0.0;
    int nn = 0;

    Rs = Element->CrossSectionRadius;
    Ra = Element->AnnularRadius;
    Xo = PosLoc[0];
    Yo = PosLoc[1];
    Zo = PosLoc[2];
    Epsilon = CosLoc[0];
    Eta = CosLoc[1];
    Rho = CosLoc[2];
    nn = 4;
    *ErrorFlag = 0;

    for (int i = 0; i < 5; i++)
    {
        rvector[i] = 0.0;
        for (int j = 0; j < 5; j++)
            amatrix[i][j] = 0.0;
    }

    amatrix[nn][4] = pow(Epsilon, 4) + 2.0 * Epsilon * Epsilon * (Eta * Eta + Rho * Rho) +
                     pow(Eta, 4) + 2.0 * Eta * Eta * Rho * Rho + pow(Rho, 4);

    amatrix[nn][3] = 4.0 * (Epsilon * Epsilon + Eta * Eta + Rho * Rho) * (Epsilon * Xo + Eta * Yo + Rho * Zo - Rho * Rs);

    amatrix[nn][2] = Xo * Xo * (6.0 * Epsilon * Epsilon + 2.0 * Eta * Eta + 2.0 * Rho * Rho) +
                     8.0 * Epsilon * Xo * (Eta * Yo + Rho * Zo - Rho * Rs) +
                     2.0 * Yo * Yo * (Epsilon * Epsilon + 3.0 * Eta * Eta + Rho * Rho) +
                     8.0 * Eta * Rho * Yo * (Zo - Rs) +
                     (Epsilon * Epsilon + Eta * Eta + 3.0 * Rho * Rho) * (2.0 * Zo * Zo - 4.0 * Rs * Zo) -
                     2.0 * Ra * Ra * (Epsilon * Epsilon + Eta * Eta - Rho * Rho) + 4.0 * Rho * Rho * Rs * Rs;

    amatrix[nn][1] = 4.0 * (Xo * Xo * (Epsilon * Xo + Eta * Yo + Rho * Zo - Rho * Rs) + Yo * Yo * (Epsilon * Xo + Eta * Yo + Rho * Zo - Rho * Rs) + Zo * Zo * (Epsilon * Xo + Eta * Yo + Rho * Zo - 3.0 * Rho * Rs) -
                            2.0 * Epsilon * Rs * Xo * Zo - Epsilon * Ra * Ra * Xo - 2.0 * Eta * Rs * Yo * Zo -
                            Eta * Ra * Ra * Yo + Rho * Ra * Ra * (Zo - Rs) + 2.0 * Rho * Rs * Rs * Zo);

    amatrix[nn][0] = pow(Xo, 4) + 2.0 * Xo * Xo * (Yo * Yo + Zo * Zo - 2.0 * Rs * Zo - Ra * Ra) +
                     pow(Yo, 4) + 2.0 * Yo * Yo * (Zo * Zo - 2.0 * Rs * Zo - Ra * Ra) +
                     pow(Zo, 4) - 4.0 * Rs * Zo * Zo * Zo + 2.0 * Ra * Ra * Zo * Zo + 4.0 * Rs * Rs * Zo * Zo - 4.0 * Ra * Ra * Rs * Zo + pow(Ra, 4);

    Root_432(nn, amatrix, rvector, &imagroot1, &imagroot2);

    // {pass results}
    if (imagroot1 == 0.0 && imagroot2 == 0.0)
    {
        // four real roots rvector[1-4] which are the pathlengths
        piksrt(4, rvector);    // sort them
        if (rvector[4] <= 0.0) // ray heading away from surface
        {
            *PathLength = rvector[4];
            *ErrorFlag = 1;
            return;
        }
        //*PathLength = rvector[4];
        // if (rvector[3] <= 0.0 )
        //	goto Label_10;
        // else
        //	*PathLength = rvector[3];
        //
        // if (rvector[2] <= 0.0 )
        //	goto Label_10;
        // else
        //	*PathLength = rvector[2];
        //
        // if (rvector[1] <= 0.0 )
        //	goto Label_10;
        // else
        //	*PathLength = rvector[1];

        if (rvector[3] > 0)
        {
            *PathLength = rvector[3];
            if (rvector[2] > 0)
            {
                *PathLength = rvector[2];
                if (rvector[1] > 0)
                {
                    *PathLength = rvector[1];
                }
            }
        }

        // goto Label_10;
    }

    else if (imagroot1 == 0.0 && imagroot2 != 0.0)
    {
        // two real roots rvector[1-2] which are the pathlengths
        if (rvector[1] <= rvector[2])
            *PathLength = rvector[1];
        else
            *PathLength = rvector[2];
        // goto Label_10;
    }

    else if (imagroot1 != 0.0 && imagroot2 == 0.0)
    {
        // two real roots rvector[3-4] which are the pathlengths
        if (rvector[3] <= rvector[4])
            *PathLength = rvector[3];
        else
            *PathLength = rvector[4];

        // goto Label_10;
    }

    else
    {
        *PathLength = 0.0;
    }

    // Label_10:
    if (*PathLength == 0.0) // ray missed torus completely
    {
        *ErrorFlag = 1;
        return;
    }

    X = Xo + *PathLength * Epsilon;
    Y = Yo + *PathLength * Eta;
    Z = Zo + *PathLength * Rho;
    Fx = -2.0 * X * (Ra - sqrt(X * X + Y * Y)) / sqrt(X * X + Y * Y);
    Fy = -2.0 * Y * (Ra - sqrt(X * X + Y * Y)) / sqrt(X * X + Y * Y);
    Fz = 2.0 * (Z - Rs);
    PosXYZ[0] = X;
    PosXYZ[1] = Y;
    PosXYZ[2] = Z;
    DFXYZ[0] = -Fx;
    DFXYZ[1] = -Fy;
    DFXYZ[2] = -Fz;
}

void QuadricSurfaceClosedForm(
    TElement *Element,
    double PosLoc[3],
    double CosLoc[3],
    double PosXYZ[3],
    double DFXYZ[3],
    double *PathLength,
    int *ErrorFlag)
{
    double Xdelta = 0.0, Ydelta = 0.0, Zdelta = 0.0;
    double Xc = 0, Yc = 0, Zc = 0, Kx = 0, Ky = 0, Kz = 0;
    double r = 0.0, r2 = 0.0, a2 = 0, b2 = 0, c2 = 0;
    double t1 = 0.0, t2 = 0.0, A = 0, B = 0, C = 0, slopemag = 0.0;

    *ErrorFlag = 0;

    switch (Element->SurfaceIndex)
    {
    case 's':
    case 'S': // sphere
        a2 = 1;
        b2 = 1;
        c2 = 1;
        Kx = 1;
        if (Element->SurfaceType == 7) // Single-axis curvature aperture
            Ky = 0;
        else
            Ky = 1;
        Kz = 1;
        r = 1.0 / Element->VertexCurvX;
        r2 = r * r;
        Xc = 0.0;
        Yc = 0.0;
        Zc = r;

        Xdelta = PosLoc[0] - Xc;
        Ydelta = PosLoc[1] - Yc;
        Zdelta = PosLoc[2] - Zc;

        A = CosLoc[0] * CosLoc[0] * Kx / a2 + CosLoc[1] * CosLoc[1] * Ky / b2 + CosLoc[2] * CosLoc[2] * Kz / c2;
        B = 2.0 * (Kx * Xdelta * CosLoc[0] / a2 + Ky * Ydelta * CosLoc[1] / b2 + Kz * Zdelta * CosLoc[2] / c2);
        C = Kx * Xdelta * Xdelta / a2 + Ky * Ydelta * Ydelta / b2 + Kz * Zdelta * Zdelta / c2 - r2;
        break;

    case 'p':
    case 'P': // parabola
        a2 = 2.0 / Element->VertexCurvX;
        b2 = Element->VertexCurvY > 0.0 ? 2.0 / Element->VertexCurvY : 1e10;
        c2 = 1.0;

        Kx = 1;
        if (Element->SurfaceType == 7) // Single-axis curvature aperture
            Ky = 0;
        else
            Ky = 1;
        Kz = 0;

        Xc = 0.0;
        Yc = 0.0;
        Zc = 0.0;

        Xdelta = PosLoc[0] - Xc;
        Ydelta = PosLoc[1] - Yc;
        Zdelta = PosLoc[2] - Zc;

        A = CosLoc[0] * CosLoc[0] * Kx / a2 + CosLoc[1] * CosLoc[1] * Ky / b2; // Note A can be zero
        B = 2.0 * (Kx * Xdelta * CosLoc[0] / a2 + Ky * Ydelta * CosLoc[1] / b2) - CosLoc[2] / c2;
        C = Kx * Xdelta * Xdelta / a2 + Ky * Ydelta * Ydelta / b2 - Zdelta / c2;

        break;

    case 'o':
    case 'O': // other
        a2 = 1;
        b2 = 1;
        c2 = 1;
        Kx = 1;
        Ky = 1;
        Kz = Element->Kappa;
        Xc = 0.0;
        Yc = 0.0;
        Zc = 1.0 / Element->Kappa / Element->VertexCurvX; // VertexCurvX = VertexCurvY for this surface type

        Xdelta = PosLoc[0] - Xc;
        Ydelta = PosLoc[1] - Yc;
        Zdelta = PosLoc[2] - Zc;

        A = CosLoc[0] * CosLoc[0] * Kx / a2 + CosLoc[1] * CosLoc[1] * Ky / b2 + CosLoc[2] * CosLoc[2] * Kz / c2;
        B = 2.0 * (Kx * Xdelta * CosLoc[0] / a2 + Ky * Ydelta * CosLoc[1] / b2 + Kz * Zdelta * CosLoc[2] / c2);
        C = Kx * Xdelta * Xdelta / a2 + Ky * Ydelta * Ydelta / b2 + Kz * Zdelta * Zdelta / c2 - 1.0 / Element->Kappa / (Element->VertexCurvX * Element->VertexCurvX);
        break;

    case 't':
    case 'T': // cylinder
        a2 = 1;
        b2 = 1;
        c2 = 1;
        Kx = 1;
        Ky = 0;
        Kz = 1;
        r = 1.0 / Element->CurvOfRev;
        r2 = r * r;
        Xc = 0.0;
        Yc = 0.0;
        Zc = r;

        Xdelta = PosLoc[0] - Xc;
        Ydelta = PosLoc[1] - Yc;
        Zdelta = PosLoc[2] - Zc;

        A = CosLoc[0] * CosLoc[0] * Kx / a2 + CosLoc[1] * CosLoc[1] * Ky / b2 + CosLoc[2] * CosLoc[2] * Kz / c2;
        B = 2.0 * (Kx * Xdelta * CosLoc[0] / a2 + Ky * Ydelta * CosLoc[1] / b2 + Kz * Zdelta * CosLoc[2] / c2);
        C = Kx * Xdelta * Xdelta / a2 + Ky * Ydelta * Ydelta / a2 + Kz * Zdelta * Zdelta / c2 - r2;
        break;

    case 'c':
    case 'C': // cone
        break;

    case 'f':
    case 'F': // flat
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

            //*************************************************************************************************************
            // makes sure to get shortest ray path on valid side of surface; 10-05-10    for open surface of parabola
            // if cylinder, then PosXYZ[3] will always be less than or equal to Element.Zaperture so never passes this test.
            // Test for  cylinder follows below.
            if (PosXYZ[2] > Element->ZAperture)
            {
                PosXYZ[0] = PosLoc[0] + t1 * CosLoc[0];
                PosXYZ[1] = PosLoc[1] + t1 * CosLoc[1];
                PosXYZ[2] = PosLoc[2] + t1 * CosLoc[2];
                *PathLength = t1;
            }

            // Remember at this point, intersection is being found on an INFINITELY
            // long cylinder. If 1st intersection on INFINITELY long cylinder is
            // from the outside, t2, check to make sure intersection is within the
            // finite length of the actual cylinder geometry, if not then 2nd
            // intersection on the inside, t1, is valid one to use.  This
            // means ray could enter from the open end  of the cylinder and hit
            // on the inside.  The final test for this is in the calling routine:
            // DetermineElementIntersectionNew
            // Wendelin 10-05-10
            if ((Element->SurfaceIndex == 't') || (Element->SurfaceIndex == 'T'))
            {
                // TODO: Figure out what to do for below when implementing Cylinder
                // surface class and SingleAxisCurvature aperture class
                // if ((PosXYZ[1] < -Element->ParameterC / 2.0) ||
                //     (PosXYZ[1] > Element->ParameterC / 2.0))
                // {
                //     PosXYZ[0] = PosLoc[0] + t1 * CosLoc[0];
                //     PosXYZ[1] = PosLoc[1] + t1 * CosLoc[1];
                //     PosXYZ[2] = PosLoc[2] + t1 * CosLoc[2];
                //     *PathLength = t1;
                // }
            }

            // Partial cylinder (sphere with single-axis curvature) needs the
            // same check as cylinder. Two intersections are possible, check
            // if the first intersection along the ray path occurs within the
            // length bounds and, if not, return the second intersection. The
            // final test for a positive ray path and a valid intersection
            // location is in DetermineElementIntersectionNew. If t1 is
            // negative, this intersection location will be ignored in
            // DetermineElementIntersectionNew
            if ((Element->SurfaceIndex == 's' || Element->SurfaceIndex == 'S') && (Element->SurfaceType == 7))
            {
                // TODO: Figure out what to do for below when implementing Cylinder
                // surface class and SingleAxisCurvature aperture class
                // if ((PosXYZ[1] < -Element->ParameterC / 2.0) || (PosXYZ[1] > Element->ParameterC / 2.0))
                // {
                //     PosXYZ[0] = PosLoc[0] + t1 * CosLoc[0];
                //     PosXYZ[1] = PosLoc[1] + t1 * CosLoc[1];
                //     PosXYZ[2] = PosLoc[2] + t1 * CosLoc[2];
                //     *PathLength = t1;
                // }
            }
            //***********************************************************************************************************

            // goto Label_100;
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

    if (Element->SurfaceIndex == 'p' || Element->SurfaceIndex == 'P')
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

int intri(double x1, double y1,
          double x2, double y2,
          double x3, double y3,
          double xt, double yt)
{
    double a = (x1 - xt) * (y2 - yt) - (x2 - xt) * (y1 - yt);
    double b = (x2 - xt) * (y3 - yt) - (x3 - xt) * (y2 - yt);
    double c = (x3 - xt) * (y1 - yt) - (x1 - xt) * (y3 - yt);
    return (sign(a) == sign(b) && sign(b) == sign(c));
}

int inquad(double x1, double y1,
           double x2, double y2,
           double x3, double y3,
           double x4, double y4,
           double xt, double yt)
{
    return intri(x1, y1, x2, y2, x3, y3, xt, yt) || intri(x1, y1, x3, y3, x4, y4, xt, yt);
}

void piksrt(int n, double arr[5])
{
	int i;
	for (int j = 1; j < n; j++)
	{
		double a = arr[j];
		for (i = j - 1; i >= 0; i--)
		{
			if (arr[i] <= a)
				goto Label_10;
			arr[i + 1] = arr[i];
		}
		i = 0;
	Label_10:
		arr[i + 1] = a;
	}
}

bool splint(std::vector<double> &xa,
            std::vector<double> &ya,
            std::vector<double> &y2a,
            int n,
            double x,
            double *y,
            double *dydx)
{
    int klo = 0, khi = 0, k = 0;
    double h = 0.0, b = 0.0, a = 0.0;

    klo = 0;
    khi = n - 1;
    while (khi - klo > 1)
    {
        k = (khi + klo) / 2;
        if (xa[k] > x)
            khi = k;
        else
            klo = k;
    }

    h = xa[khi] - xa[klo];
    if (h != 0.0)
    {

        a = (xa[khi] - x) / h;
        b = (x - xa[klo]) / h;
        *y = a * ya[klo] + b * ya[khi] +
             ((a * a * a - a) * y2a[klo] + (b * b * b - b) * y2a[khi]) * (h * h) / 6.0;
        *dydx = (ya[khi] - ya[klo]) / (xa[khi] - xa[klo]) -
                (3.0 * a * a - 1.0) * (xa[khi] - xa[klo]) * y2a[klo] / 6.0 + 
                (3.0 * b * b - 1.0) * (xa[khi] - xa[klo]) * y2a[khi] / 6.0;
    }
    else
    {
        return false;
    }

    return true;
}


#undef sign
#undef sqr
