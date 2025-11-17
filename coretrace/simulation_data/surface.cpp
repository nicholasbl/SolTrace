
#include "surface.hpp"

#include <vector>

namespace SolTrace::Data {

surface_ptr make_surface_from_type(SurfaceType type, const std::vector<double> &args)
{
    surface_ptr retval = nullptr;
    unsigned nargs = args.size();

    switch (type)
    {
    case SurfaceType::CONE:
        retval = nargs < 1 ? nullptr : make_surface<Cone>(args[0]);
        break;
    case SurfaceType::CYLINDER:
        retval = nargs < 1 ? nullptr : make_surface<Cylinder>(1.0 / args[0]);
        break;
    case SurfaceType::FLAT:
        retval = make_surface<Flat>();
        break;
    case SurfaceType::PARABOLA:
    {
        if (nargs < 2)
            retval = nullptr;
        else
        {
            double cx = args[0];
            double cy = args[1];
            double fx = 1.0 / (2.0 * cx);
            double fy = 1.0 / (2.0 * cy);
            retval = make_surface<Parabola>(fx, fy);
        }
        break;
    } 
    case SurfaceType::SPHERE:
        retval = nargs < 1 ? nullptr : make_surface<Sphere>(args[0]);
        break;
    case SurfaceType::HYPER:
    case SurfaceType::GENERAL_SPENCER_MURTY:
    case SurfaceType::TORUS:
    default:
        retval = nullptr; // Not implemented yet
        break;
    }

    return retval;
}

double Cone::z(double x, double y) const {
    return sqrt(x * x + y * y) / tan(half_angle);
}
double Cylinder::z(double x, double) const {
    // TODO: Fix ? This is really only the top half of the cylinder
    //       Clyinder breaks the model since it is a mulit-valued fuction: each
    //       x values produces two z values Returning only the positive root
    return radius + sqrt(x * x + radius * radius);
}
double Parabola::z(double x, double y) const {
    // z(x,y) = (cx * x^2 + cy * y^2) / 2
    return x * x / focal_length_x + y * y / focal_length_y;
}
double Sphere::z(double x, double y) const {
    // TODO: Double-check: This provides only the bottom-half of the sphere,
    // aligning with the image in the documentation
    // TODO: Verify how vertex_curv relates to radius, i.e., vertex_curve = 1/r?
    return vertex_curv * (x * x + y * y) /
           (1 + sqrt(1 - vertex_curv * vertex_curv * (x * x + y * y)));
}

} // namespace SolTrace::Data
