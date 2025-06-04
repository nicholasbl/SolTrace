
#include "calculator_factory.hpp"

#include "simulation_data/surface.hpp"

#include "flat_calculator.hpp"
#include "newton_calculator.hpp"
#include "quadric_calculator.hpp"

// std::map<SurfaceType,

CalculatorFactory* CalculatorFactory::instance = nullptr;

calculator_ptr CalculatorFactory::make_calculator(surface_ptr surf)
{
    // TODO: Rework without a if-else tree?
    SurfaceType st = surf->get_type();
    calculator_ptr calc = nullptr;
    if (st == PARABOLA)
    {
        calc = std::make_shared<QuadricCalculator>(surf);
    }
    else if(st == FLAT)
    {
        calc = std::make_shared<FlatCalculator>(surf);
    }
    else
    {
        // TODO: Error message here?
    }
    return calc;
}
