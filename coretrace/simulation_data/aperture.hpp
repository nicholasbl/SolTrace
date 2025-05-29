#ifndef SOLTRACE_APERTURE_H
#define SOLTRACE_APERTURE_H

#include <cmath>
#include <memory>

// TODO: Make a header file for constants...
#ifndef M_PI
#define M_PI 3.141592653589793238462643
#endif

enum ApertureType
{
    ANNULUS,
    CIRCLE,
    HEXAGON,
    RECTANGLE,
    EQUILATERAL_TRIANGLE,
    SINGLE_AXIS_CURVATURE_SECTION,
    IRREGULAR_TRIANGLE,
    IRREGULAR_QUADRILATERAL
};

struct Aperture;
using aperture_ptr = std::shared_ptr<Aperture>;

template <typename A, typename... Args>
inline auto make_aperture(Args &&...args)
{
    return std::make_shared<A>(std::forward<Args>(args)...);
}

struct Aperture
{
public:
    ApertureType my_type;

    Aperture(ApertureType type) : my_type(type) {}
    virtual ~Aperture() {}

    inline ApertureType get_type() const
    {
        return my_type;
    }
    virtual inline double radius_circumscribed_circle() const
    {
        return 0.5 * this->diameter_circumscribed_circle();
    }

    virtual double aperture_area() const = 0;
    virtual double diameter_circumscribed_circle() const = 0;
    virtual bool is_in(double x, double y) const = 0;

    virtual aperture_ptr make_copy() const = 0;

    // virtual Aperture& operator=(const Aperture &rhs)
    // {
    //     this->my_type = rhs.my_type;
    //     return *this;
    // }
};

struct Annulus : public Aperture
{
    double inner_radius;
    double outer_radius;
    double arc_angle;

    Annulus()
        : Aperture(ANNULUS),
          inner_radius(0.0), outer_radius(0.0), arc_angle(0.0)
    {
    }
    Annulus(double ri, double ro, double arc)
        : Aperture(ANNULUS),
          inner_radius(ri), outer_radius(ro), arc_angle(arc)
    {
    }
    virtual ~Annulus() {}

    virtual double aperture_area() const
    {
        // TODO: input.cpp on line 219 uses the formula
        //    elm->ParameterC*(ACOSM1O180)*(elm->ParameterB - elm->ParameterA);
        //    = \theta * (r - R)
        // This seems to be wrong...
        double R = this->outer_radius;
        double r = this->inner_radius;
        // double arc = this->arc_angle * 180.0 / M_PI;
        double arc = this->arc_angle * M_PI / 180.0;
        return 0.5 * M_PI * arc * (R * R - r * r);
        // Below should be more efficient but is less readable...
        // static const double coef = M_PI * M_PI / 90.0;
        // double arc = this->arc_angle;
        // return coef * arc * (R * R - r * r);
    }

    virtual double diameter_circumscribed_circle() const
    {
        return 2.0 * this->outer_radius;
    }

    virtual bool is_in(double x, double y) const
    {
        double r = sqrt(x * x + y * y);
        return (this->inner_radius <= r &&
                r <= this->outer_radius);
    }

    virtual aperture_ptr make_copy() const
    {
        // Invokes the implicit copy constructor
        return make_aperture<Annulus>(*this);
    }

    // virtual Annulus& operator=(const Annulus &rhs)
    // {
    //     Aperture::operator=(rhs);
    //     this->inner_radius = rhs.inner_radius;
    //     this->outer_radius = rhs.outer_radius;
    //     this->arc_angle = rhs.arc_angle;
    //     return *this;
    // }
};

struct Circle : public Aperture
{
    double diameter;

    Circle() : Aperture(CIRCLE), diameter(0.0) {}
    Circle(double d) : Aperture(CIRCLE), diameter(d) {}
    virtual ~Circle() {}

    virtual double aperture_area() const
    {
        return 0.25 * M_PI * this->diameter * this->diameter;
    }

    virtual double diameter_circumscribed_circle() const
    {
        return this->diameter;
    }

    virtual bool is_in(double x, double y) const
    {
        double r = sqrt(x * x + y * y);
        return r <= 0.5 * this->diameter;
    }

    virtual aperture_ptr make_copy() const
    {
        // Invokes the implicit copy constructor
        return make_aperture<Circle>(*this);
    }

    // virtual Circle& operator=(const Circle &rhs)
    // {
    //     Aperture::operator=(rhs);
    //     this->diameter = rhs.diameter;
    //     return *this;
    // }
};

struct EqualateralTriangle : public Aperture
{
    double circumscribe_diameter;
    EqualateralTriangle() : Aperture(EQUILATERAL_TRIANGLE),
                            circumscribe_diameter(0.0)
    {
    }
    EqualateralTriangle(double cd) : Aperture(EQUILATERAL_TRIANGLE),
                                     circumscribe_diameter(cd)
    {
    }
    virtual ~EqualateralTriangle() {}

    virtual double aperture_area() const
    {
        double r = 0.5 * this->circumscribe_diameter;
        return 0.75 * sqrt(3) * r * r;
    }

    virtual double diameter_circumscribed_circle() const
    {
        return this->circumscribe_diameter;
    }

    virtual bool is_in(double x, double y) const;

    virtual aperture_ptr make_copy() const
    {
        // Invokes the implicit copy constructor
        return make_aperture<EqualateralTriangle>(*this);
    }

    // virtual EqualateralTriangle& operator=(const EqualateralTriangle &rhs)
    // {
    //     Aperture::operator=(rhs);
    //     this->circumscribe_diameter = rhs.circumscribe_diameter;
    //     return *this;
    // }
};

struct Hexagon : public Aperture
{
    double circumscribe_diameter;

    Hexagon() : Aperture(HEXAGON), circumscribe_diameter(0.0) {}
    Hexagon(double d) : Aperture(HEXAGON), circumscribe_diameter(d) {}
    virtual ~Hexagon() {}
    virtual double aperture_area() const
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

    virtual double diameter_circumscribed_circle() const
    {
        return circumscribe_diameter;
    }

    virtual bool is_in(double x, double y) const;

    virtual aperture_ptr make_copy() const
    {
        // Invokes the implicit copy constructor
        return make_aperture<Hexagon>(*this);
    }

    // virtual Hexagon& operator=(const Hexagon &rhs)
    // {
    //     Aperture::operator=(rhs);
    //     this->circumscribe_diameter = rhs.circumscribe_diameter;
    //     return *this;
    // }
};

struct Rectangle : public Aperture
{
    double x_length;
    double y_length;
    Rectangle() : Aperture(RECTANGLE),
                  x_length(0.0),
                  y_length(0.0)
    {
    }
    Rectangle(double xlen, double ylen) : Aperture(RECTANGLE),
                                          x_length(xlen),
                                          y_length(ylen)
    {
    }
    virtual ~Rectangle() {}

    virtual double aperture_area() const
    {
        return this->x_length * this->y_length;
    }

    virtual double diameter_circumscribed_circle() const
    {
        return sqrt(x_length * x_length + y_length * y_length);
    }

    virtual bool is_in(double x, double y) const
    {
        double xh = 0.5 * this->x_length;
        double yh = 0.5 * this->y_length;
        return (-xh <= x && x <= xh && -yh <= y && y <= yh);
    }

    virtual aperture_ptr make_copy() const
    {
        // Invokes the implicit copy constructor
        return make_aperture<Rectangle>(*this);
    }

    // virtual Rectangle& operator=(const Rectangle &rhs)
    // {
    //     Aperture::operator=(rhs);
    //     this->x_length = rhs.x_length;
    //     this->y_length = rhs.y_length;
    //     return *this;
    // }
};

// TODO: Implement the below cases

struct SingleAxisCurvatureSection : public Aperture
{
};

struct IrregularTriangle : public Aperture
{
};

struct IrregularQuadrilateral : public Aperture
{
};

#endif
