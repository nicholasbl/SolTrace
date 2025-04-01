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
    Annular(double ri, double ro, double arc)
        : inner_radius(ri), outer_radius(ro), arc_angle(arc)
    {
    }
};

struct Circle : public Aperature
{
    double diameter;
    Circle(double d) : diameter(d) {}
};

struct Hexagonal : public Aperature
{
};

struct Rectangle : public Aperature
{
    double height;
    double width;
    Rectangle(double h, double w) : height(h), width(w) {}
};

struct Triangular : public Aperature
{
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

// {
// // public:
// //     Rectangle(){}
// //     virtual ~Rectangle(){}
// };

#endif
