
#include "calculator_factory.hpp"

#include "cylinder_calculator.hpp"
#include "flat_calculator.hpp"
#include "native_runner_types.hpp"
#include "newton_calculator.hpp"
#include "parabola_calculator.hpp"
#include "sphere_calculator.hpp"
#include "surface.hpp"

// std::map<SurfaceType,

CalculatorFactory *CalculatorFactory::instance = nullptr;

calculator_ptr CalculatorFactory::make_calculator(
    aperture_ptr ap,
    surface_ptr surf,
    const ElementParameters &eparams)
{
    // TODO: Rework without an if-else tree?
    SurfaceType st = surf->get_type();
    calculator_ptr calc = nullptr;
    if (st == PARABOLA)
    {
        // calc = std::make_shared<QuadricCalculator>(surf);
        calc = std::make_shared<ParabolaCalculator>(surf);
    }
    else if (st == FLAT)
    {
        calc = std::make_shared<FlatCalculator>(surf);
    }
    else if (st == CYLINDER)
    {
        calc = std::make_shared<CylinderCalculator>(surf, ap);
    }
    else if (st == SPHERE)
    {
        calc = std::make_shared<SphereCalculator>(surf);
    }
    else
    {
        // TODO: Error message here!
        assert(false);
    }
    return calc;
}
