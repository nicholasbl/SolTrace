
#include "aperture.hpp"

#include <cmath>

bool EqualateralTriangle::is_in(double x, double y) const
{
    double r = sqrt(x * x + y * y);
    double ro = this->radius_circumscribed_circle();
    if (r > ro)
        return false;

    double ri = 0.5 * ro;
    if (r <= ri)
        return true;

    double y0;
    // double a = ro / sqrt(3.0) = 2 * ri / sqrt(3.0);
    if (0.0 <= x && x <= ro)
    {
        // y0 = -sqrt(3.0) * (x - a);
        y0 = ro - sqrt(3.0) * x;
        return (-ri <= y && y <= y0);
    }
    else if (-ro <= x && x < 0.0)
    {
        // y0 = sqrt(3.0) * (x + a);
        y0 = sqrt(3.0) * x + ro;
        return (-ri <= y && y <= y0);
    }

    return false;
}

bool Hexagon::is_in(double x, double y) const
{
    double r = sqrt(x * x + y * y);
    double ro = this->radius_circumscribed_circle();
    if (r > ro)
        return false;

    double ri = 0.5 * sqrt(3.0) * ro;
    if (r <= ri)
        return true;

    // NOTE: Old code used
    //    xl = sqrt(ro^2 - ri^2)
    // where `ro` is the radius of the circumscribing circle and `ri` is
    // the radius of the inscribing circle. But this is equivalent to
    //    xi = 0.5 * ro

    double xl = 0.5 * this->radius_circumscribed_circle();
    double y1, y2;
    if (xl < x && x <= ro)
    {
        y1 = sqrt(3.0) * (x - ro);
        y2 = -y1;
        // if (y1 <= y && y <= y2) return true;
        return (y1 <= y && y <= y2);
    }
    else if (-xl <= x && x <= xl)
    {
        return (-ri <= y && y <= ri);
    }
    else if (-ro <= x && x < xl)
    {
        y1 = sqrt(3.0) * (x + ro);
        y2 = -y1;
        return (y2 <= y && y <= y1);
    }

    return false;
}
