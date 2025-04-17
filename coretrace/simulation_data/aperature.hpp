#ifndef SOLTRACE_SHAPE_H
#define SOLTRACE_SHAPE_H

enum AperatureType
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

struct Aperature
{
public:
    AperatureType my_type;

    Aperature(AperatureType type) : my_type(type) {}
    virtual ~Aperature() {}

    AperatureType get_type()
    {
        return my_type;
    }
};

struct Annular : public Aperature
{
    double inner_radius;
    double outer_radius;
    double arc_angle;
    Annular()
        : Aperature(ANNULAR),
          inner_radius(0.0), outer_radius(0.0), arc_angle(0.0)
    {
    }
    Annular(double ri, double ro, double arc)
        : Aperature(ANNULAR),
          inner_radius(ri), outer_radius(ro), arc_angle(arc)
    {
    }
    virtual ~Annular() {}
};

struct Circular : public Aperature
{
    double diameter;
    Circular() : Aperature(CIRCULAR), diameter(0.0) {}
    Circular(double d) : Aperature(CIRCULAR), diameter(d) {}
    virtual ~Circular() {}
};

struct Hexagonal : public Aperature
{
    double circumscribe_diameter;
    Hexagonal() : Aperature(HEXAGONAL), circumscribe_diameter(0.0) {}
    Hexagonal(double d) : Aperature(HEXAGONAL), circumscribe_diameter(d) {}
    virtual ~Hexagonal() {}
};

struct Rectangular : public Aperature
{
    double height;
    double width;
    Rectangular() : Aperature(RECTANGULAR),
                    height(0.0), width(0.0) {}
    Rectangular(double h, double w) : Aperature(RECTANGULAR),
                                      height(h), width(w) {}
    virtual ~Rectangular() {}
};

struct Triangular : public Aperature
{
    double circumscribe_diameter;
    Triangular() : Aperature(TRIANGULAR),
                   circumscribe_diameter(0.0) {}
    Triangular(double cd) : Aperature(TRIANGULAR),
                            circumscribe_diameter(cd) {}
    virtual ~Triangular() {}
};

struct SingleAxisCurvatureSection : public Aperature
{
};

struct IrregularTriangle : public Aperature
{
};

struct IrregularQuadrilateral : public Aperature
{
};

using aperature_ptr = std::shared_ptr<Aperature>;

template <typename A, typename... Args>
inline auto make_aperature(Args &&...args)
{
    return std::make_shared<A>(std::forward<Args>(args)...);
}

#endif
