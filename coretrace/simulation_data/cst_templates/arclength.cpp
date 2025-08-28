
#include "arclength.hpp"

#include <cmath>
#include <cstdint>

double parabolic_arc_length(double cx, double x)
{
    double cxx = cx * x;
    return sqrt(1.0 + cxx * cxx);
}

double parabolic_arc_length(double cx, double x0, double x1, double dx)
{
    int_fast64_t count = 0;
    int_fast64_t nsteps = static_cast<int_fast64_t>(floor((x1 - x0) / dx));

    double xk = x0;
    double yk = parabolic_arc_length(cx, xk);
    double s = 0.5 * yk * dx;
    while (count < nsteps)
    {
        ++count;
        xk = x0 + count * dx;
        yk = parabolic_arc_length(cx, xk);
        s += yk * dx;
    }
    yk = parabolic_arc_length(cx, x1);
    s += 0.5 * yk * (x1 - x0 - nsteps*dx);

    return s;
}

double parabolic_determine_x_coordinate(double cx,
                                        double x0,
                                        double arc_length,
                                        double dx)
{
    double x1 = x0;
    double y1 = parabolic_arc_length(cx, x1);
    double s = 0.5 * y1 * dx;
    double area;

    while (true)
    {
        x1 += dx;
        y1 = parabolic_arc_length(cx, x1);
        area = 0.5 * y1 * dx;
        s += area;
        if (s < arc_length)
        {
            s += area;
        }
        else
        {
            break;
        }
    }

    return x1;
}
