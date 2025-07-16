#ifndef SOLTRACE_APERTURE_H
#define SOLTRACE_APERTURE_H

#include <cmath>
#include <memory>

// TODO: Make a header file for constants...
#ifndef M_PI
#define M_PI 3.141592653589793238462643
#endif

// TODO: For apertures that do not include the origin, should the
// "circumscribing" circle be centered at the origin? Or should it
// be the actual circumscribed circle.

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
};

struct Annulus : public Aperture
{
    double inner_radius;
    double outer_radius;
    double arc_angle;

    // Annulus()
    //     : Aperture(ANNULUS),
    //       inner_radius(0.0), outer_radius(0.0), arc_angle(0.0)
    // {
    // }
    Annulus(double ri, double ro, double arc)
        : Aperture(ANNULUS),
          inner_radius(ri), outer_radius(ro), arc_angle(arc)
    {
    }
    virtual ~Annulus() {}

    virtual double aperture_area() const;
    virtual double diameter_circumscribed_circle() const;
    virtual bool is_in(double x, double y) const;
    virtual aperture_ptr make_copy() const;
};

struct Circle : public Aperture
{
    double diameter;

    // Circle() : Aperture(CIRCLE), diameter(0.0) {}
    Circle(double d) : Aperture(CIRCLE), diameter(d) {}
    virtual ~Circle() {}

    virtual double aperture_area() const;
    virtual double diameter_circumscribed_circle() const;
    virtual bool is_in(double x, double y) const;
    virtual aperture_ptr make_copy() const;
};

struct EqualateralTriangle : public Aperture
{
    double circumscribe_diameter;
    // EqualateralTriangle() : Aperture(EQUILATERAL_TRIANGLE),
    //                         circumscribe_diameter(0.0)
    // {
    // }
    EqualateralTriangle(double cd) : Aperture(EQUILATERAL_TRIANGLE),
                                     circumscribe_diameter(cd)
    {
    }
    virtual ~EqualateralTriangle() {}

    virtual double aperture_area() const;
    virtual double diameter_circumscribed_circle() const;
    virtual bool is_in(double x, double y) const;
    virtual aperture_ptr make_copy() const;
};

struct Hexagon : public Aperture
{
    double circumscribe_diameter;

    // Hexagon() : Aperture(HEXAGON), circumscribe_diameter(0.0) {}
    Hexagon(double d) : Aperture(HEXAGON), circumscribe_diameter(d) {}
    virtual ~Hexagon() {}
    virtual double aperture_area() const;
    virtual double diameter_circumscribed_circle() const;
    virtual bool is_in(double x, double y) const;
    virtual aperture_ptr make_copy() const;
};

struct Rectangle : public Aperture
{
    double x_length;
    double y_length;
    // NOTE: The point (x_coord, y_coord) gives the location of the
    // lower left hand corner of the rectangle in the xy-plane.
    double x_coord;
    double y_coord;
    Rectangle(double xlen, double ylen)
        : Aperture(RECTANGLE),
          x_length(xlen),
          y_length(ylen)
    {
        // Default to rectangle centered at the origin.
        this->x_coord = -0.5 * this->x_length;
        this->y_coord = -0.5 * this->y_length;
        return;
    }
    Rectangle(double xlen, double ylen, double xl, double yl)
        : Aperture(RECTANGLE),
          x_length(xlen),
          y_length(ylen),
          x_coord(xl),
          y_coord(yl)
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
        double xl = this->x_coord;
        double yl = this->y_coord;
        double xu = xl + this->x_length;
        double yu = yl + this->y_length;
        return (xl <= x && x <= xu && yl <= y && y <= yu);
    }

    virtual aperture_ptr make_copy() const
    {
        // Invokes the implicit copy constructor
        return make_aperture<Rectangle>(*this);
    }
};

struct SingleAxisCurvatureSection : public Aperture
{
    // TODO: Implement this?
};

struct IrregularTriangle : public Aperture
{
    // Locations of the 3 vertices
    double x1;
    double y1;
    double x2;
    double y2;
    double x3;
    double y3;

    IrregularTriangle(double x1, double y1,
                      double x2, double y2,
                      double x3, double y3);
    ~IrregularTriangle() {}

    virtual double aperture_area() const;
    virtual double diameter_circumscribed_circle() const;
    virtual bool is_in(double x, double y) const;
    virtual aperture_ptr make_copy() const;
};

struct IrregularQuadrilateral : public Aperture
{
    // Locations of the 4 vertices
    double x1;
    double y1;
    double x2;
    double y2;
    double x3;
    double y3;
    double x4;
    double y4;

    IrregularQuadrilateral(double x1, double y1,
                           double x2, double y2,
                           double x3, double y3,
                           double x4, double y4);
    ~IrregularQuadrilateral() {}

    virtual double aperture_area() const;
    virtual double diameter_circumscribed_circle() const;
    virtual bool is_in(double x, double y) const;
    virtual aperture_ptr make_copy() const;
};

bool intri(double x1, double y1,
           double x2, double y2,
           double x3, double y3,
           double xt, double yt);

bool inquad(double x1, double y1,
            double x2, double y2,
            double x3, double y3,
            double x4, double y4,
            double xt, double yt);

#endif
