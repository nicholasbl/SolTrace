#ifndef SOLTRACE_CALCULATOR_FACTORY_H
#define SOLTRACE_CALCULATOR_FACTORY_H

// #include <map>
// #include <memory>

#include "aperture.hpp"
#include "surface.hpp"
#include "surface_intersection_calculator.hpp"

class CalculatorFactory
{
public:
    CalculatorFactory(const CalculatorFactory&) = delete;
    CalculatorFactory& operator=(const CalculatorFactory&) = delete;

    static CalculatorFactory* get()
    {
        if (CalculatorFactory::instance == nullptr)
        {
            // instance = std::make_unique<CalculatorFactory>();
            instance = new CalculatorFactory();
        }
        return instance;
    }

    // calculator_ptr make_calculator(aperture_ptr ap, surface_ptr surf);
    calculator_ptr make_calculator(surface_ptr surf);

private:
    // static std::unique_ptr<CalculatorFactory> instance;
    static CalculatorFactory *instance;
    // std::map<SurfaceType, calculator_ptr> class_map;

    CalculatorFactory(){}
    // ~CalculatorFactory(){}    
};

#endif
