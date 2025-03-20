#ifndef SOLTRACE_SHAPE_H
#define SOLTRACE_SHAPE_H

class Shape
{
public:
    Shape(){}
    virtual ~Shape(){}
};

class Plane: public Shape
{
public:
    Plane(){}
    virtual ~Plane(){}
};

#endif
