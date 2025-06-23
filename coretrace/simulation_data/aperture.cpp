
#include "aperture.hpp"

#include <cmath>

double Annulus::aperture_area() const
{
    // TODO: input.cpp on line 219 uses the formula
    //    elm->ParameterC*(ACOSM1O180)*(elm->ParameterB - elm->ParameterA);
    //    = \theta * (r - R)
    // This seems to be wrong...
    double R = this->outer_radius;
    double r = this->inner_radius;
    // Convert to radians
    double arc = this->arc_angle * M_PI / 180.0;
    return 0.5 * arc * (R * R - r * r);
}

double Annulus::diameter_circumscribed_circle() const
{
    return 2.0 * this->outer_radius;
}

bool Annulus::is_in(double x, double y) const
{
    double r = sqrt(x * x + y * y);
    bool inside = false;
    if (this->inner_radius <= r &&
        r <= this->outer_radius)
    {
        double theta = atan2(y, x);
        // Arc is split across x-axis, hence the 0.5
        double arc = 0.5 * this->arc_angle * M_PI / 180.0;
        inside = (-arc <= theta && theta <= arc);
    }
    return inside;
}

aperture_ptr Annulus::make_copy() const
{
    // Invokes the implicit copy constructor
    return make_aperture<Annulus>(*this);
}

double Circle::aperture_area() const
{
    return 0.25 * M_PI * this->diameter * this->diameter;
}

double Circle::diameter_circumscribed_circle() const
{
    return this->diameter;
}

bool Circle::is_in(double x, double y) const
{
    double r = sqrt(x * x + y * y);
    return r <= this->radius_circumscribed_circle();
}

aperture_ptr Circle::make_copy() const
{
    // Invokes the implicit copy constructor
    return make_aperture<Circle>(*this);
}

double EqualateralTriangle::aperture_area() const
{
    double r = 0.5 * this->circumscribe_diameter;
    return 0.75 * sqrt(3) * r * r;
}

double EqualateralTriangle::diameter_circumscribed_circle() const
{
    return this->circumscribe_diameter;
}

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

aperture_ptr EqualateralTriangle::make_copy() const
{
    // Invokes the implicit copy constructor
    return make_aperture<EqualateralTriangle>(*this);
}

double Hexagon::aperture_area() const
{
    // TODO: input.cpp on line 210 uses the formula
    //    5*sqr(elm->ParameterA/2.0)*cos(30.0*(ACOSM1O180))*sin(30.0*(ACOSM1O180));
    //    = 5*(d/2)^2*cos(pi/6)*sin(pi/6)
    //    = 5*(d/2)^2*sqrt(3)/2*1/2
    //    = 5*sqrt(3)/4 * (d/2)^2
    //    = 1.25*sqrt(3) * (d/2)^2
    // This seems to be wrong...
    double r = 0.5 * this->circumscribe_diameter;
    return 1.5 * sqrt(3) * r * r;
}

double Hexagon::diameter_circumscribed_circle() const
{
    return circumscribe_diameter;
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
    else if (-ro <= x && x < -xl)
    {
        y1 = sqrt(3.0) * (x + ro);
        y2 = -y1;
        return (y2 <= y && y <= y1);
    }

    return false;
}

aperture_ptr Hexagon::make_copy() const
{
    // Invokes the implicit copy constructor
    return make_aperture<Hexagon>(*this);
}

// int intri(double x1, double y1,
//           double x2, double y2,
//           double x3, double y3,
//           double xt, double yt)
// {
//     double a = (x1 - xt) * (y2 - yt) - (x2 - xt) * (y1 - yt);
//     double b = (x2 - xt) * (y3 - yt) - (x3 - xt) * (y2 - yt);
//     double c = (x3 - xt) * (y1 - yt) - (x1 - xt) * (y3 - yt);
//     return (sign(a) == sign(b) && sign(b) == sign(c));
// }

// int inquad(double x1, double y1,
//            double x2, double y2,
//            double x3, double y3,
//            double x4, double y4,
//            double xt, double yt)
// {
//     return intri(x1, y1, x2, y2, x3, y3, xt, yt) || intri(x1, y1, x3, y3, x4, y4, xt, yt);
// }
