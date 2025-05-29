#ifndef SOLTRACE_ELEMENT_INTERSECTION_H
#define SOLTRACE_ELEMENT_INTERSECTION_H

#include "native_runner_types.hpp"

void DetermineElementIntersectionNew(TElement *Element,
                                     double PosRayIn[3],
                                     double CosRayIn[3],
                                     double PosRayOut[3],
                                     double CosRayOut[3],
                                     double DFXYZ[3],
                                     double *PathLength,
                                     int *ErrorFlag,
                                     int *Intercept,
                                     int *BacksideFlag);

// void Intersect(double PosLoc[3],
//                double CosLoc[3],
//                TElement *Element,
//                double PosXYZ[3],
//                double CosKLM[3],
//                double DFXYZ[3],
//                double *PathLength,
//                int *ErrorFlag);

void Root_432(int order,
              double Coeffs[5][5],
              double RealRoots[5],
              double *ImRoot1,
              double *ImRoot2);

// void Surface(double PosXYZ[3],
//              TElement *Element,
//              double *FXYZ,
//              double DFXYZ[3],
//              int *ErrorFlag);

// void SurfaceZatXYPair(double PosXYZ[3],
//                       TElement *Element,
//                       double *FXYZ,
//                       int *ErrorFlag);

void TorusClosedForm(TElement *Element,
                     double PosLoc[3],
                     double CosLoc[3],
                     double PosXYZ[3],
                     double DFXYZ[3],
                     double *PathLength,
                     int *ErrorFlag);

// int intri(double x1, double y1,
//           double x2, double y2,
//           double x3, double y3,
//           double xt, double yt);

// int inquad(double x1, double y1,
//            double x2, double y2,
//            double x3, double y3,
//            double x4, double y4,
//            double xt, double yt);

void piksrt(int n, double arr[5]);

// bool splint(std::vector<double> &xa,
//             std::vector<double> &ya,
//             std::vector<double> &y2a,
//             int n,
//             double x,
//             double *y,
//             double *dydx);

#endif
