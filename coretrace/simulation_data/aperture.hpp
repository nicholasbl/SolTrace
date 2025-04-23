#ifndef SOLTRACE_APERTURE_H
#define SOLTRACE_APERTURE_H

#include <memory>

enum ApertureType
{
    ANNULAR,
    CIRCULAR,
    HEXAGONAL,
    RECTANGULAR,
    TRIANGULAR,
    SINGLE_AXIS_CURVATURE_SECTION,
    IRREGULAR_TRIANGLE,
    IRREGULAR_QUADRILATERAL
};

struct Aperture
{
public:
    ApertureType my_type;

    Aperture(ApertureType type) : my_type(type) {}
    virtual ~Aperture() {}

    ApertureType get_type()
    {
        return my_type;
    }
};

struct Annular : public Aperture
{
    double inner_radius;
    double outer_radius;
    double arc_angle;
    Annular()
        : Aperture(ANNULAR),
          inner_radius(0.0), outer_radius(0.0), arc_angle(0.0)
    {
    }
    Annular(double ri, double ro, double arc)
        : Aperture(ANNULAR),
          inner_radius(ri), outer_radius(ro), arc_angle(arc)
    {
    }
    virtual ~Annular() {}
};

struct Circular : public Aperture
{
    double diameter;
    Circular() : Aperture(CIRCULAR), diameter(0.0) {}
    Circular(double d) : Aperture(CIRCULAR), diameter(d) {}
    virtual ~Circular() {}
};

struct Hexagonal : public Aperture
{
    double circumscribe_diameter;
    Hexagonal() : Aperture(HEXAGONAL), circumscribe_diameter(0.0) {}
    Hexagonal(double d) : Aperture(HEXAGONAL), circumscribe_diameter(d) {}
    virtual ~Hexagonal() {}
};

struct Rectangular : public Aperture
{
    double x_length;
    double y_length;
    Rectangular() : Aperture(RECTANGULAR),
                    x_length(0.0), y_length(0.0) {}
    Rectangular(double xlen, double ylen) : Aperture(RECTANGULAR),
                                            x_length(xlen), y_length(ylen) {}
    virtual ~Rectangular() {}
};

struct Triangular : public Aperture
{
    double circumscribe_diameter;
    Triangular() : Aperture(TRIANGULAR),
                   circumscribe_diameter(0.0) {}
    Triangular(double cd) : Aperture(TRIANGULAR),
                            circumscribe_diameter(cd) {}
    virtual ~Triangular() {}
};

// TODO: Implement the below cases?

struct SingleAxisCurvatureSection : public Aperture
{
};

struct IrregularTriangle : public Aperture
{
};

struct IrregularQuadrilateral : public Aperture
{
};

using aperture_ptr = std::shared_ptr<Aperture>;

template <typename A, typename... Args>
inline auto make_aperture(Args &&...args)
{
    return std::make_shared<A>(std::forward<Args>(args)...);
}

#endif
