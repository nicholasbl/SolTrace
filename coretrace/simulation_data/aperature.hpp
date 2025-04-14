#ifndef SOLTRACE_SHAPE_H
#define SOLTRACE_SHAPE_H

struct Aperature
{
public:
    Aperature() {}
    virtual ~Aperature() {}
};

struct Annular : public Aperature
{
    double inner_radius;
    double outer_radius;
    double arc_angle;
    Annular() : inner_radius(0.0), outer_radius(0.0), arc_angle(0.0) {}
    Annular(double ri, double ro, double arc)
        : inner_radius(ri), outer_radius(ro), arc_angle(arc)
    {
    }
    virtual ~Annular(){}
};

struct Circular : public Aperature
{
    double diameter;
    Circular() : diameter(0.0) {}
    Circular(double d) : diameter(d) {}
    virtual ~Circular() {}
};

struct Hexagonal : public Aperature
{
    double circumscribe_diameter;
    Hexagonal() : circumscribe_diameter(0.0) {}
    Hexagonal(double d) : circumscribe_diameter(d) {}
    virtual ~Hexagonal() {}
};

struct Rectangle : public Aperature
{
    double height;
    double width;
    Rectangle() : height(0.0), width(0.0) {}
    Rectangle(double h, double w) : height(h), width(w) {}
    virtual ~Rectangle() {}
};

struct Triangular : public Aperature
{
    double circumscribe_diameter;
    Triangular() : circumscribe_diameter(0.0) {}
    Triangular(double cd) : circumscribe_diameter(cd) {}
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
inline aperature_ptr make_aperature(Args &&...args)
{
    return std::make_shared<A>(std::forward<Args>(args)...);
}

#endif
