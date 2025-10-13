
#include "surface.hpp"

#include <vector>

namespace SolTrace::Data {

surface_ptr make_surface_from_type(SurfaceType type, const std::vector<double> &args)
{
    surface_ptr retval = nullptr;
    unsigned nargs = args.size();

    switch (type)
    {
    case CONE:
        retval = nargs < 1 ? nullptr : make_surface<Cone>(args[0]);
        break;
    case CYLINDER:
        retval = nargs < 1 ? nullptr : make_surface<Cylinder>(1.0 / args[0]);
        break;
    case FLAT:
        retval = make_surface<Flat>();
        break;
    case PARABOLA:
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
    case SPHERE:
        retval = nargs < 1 ? nullptr : make_surface<Sphere>(args[0]);
        break;
    case HYPER:
    case GENERAL_SPENCER_MURTY:
    case TORUS:
    default:
        retval = nullptr; // Not implemented yet
        break;
    }

    return retval;
}

} // namespace SolTrace::Data
