#ifndef SOLTRACE_OPTICAL_PROPERTIES_H
#define SOLTRACE_OPTICAL_PROPERTIES_H

enum InteractionType
{
    REFLECTION,
    REFRACTION
};

struct OpticalProperties
{
    InteractionType my_type;
    double transmitivity;
    double reflectivity;
    double slope_error;
    double specularity_error;

    OpticalProperties() : my_type(REFLECTION),
                          transmitivity(0.0),
                          reflectivity(0.0),
                          slope_error(0.0),
                          specularity_error(0.0)
    {
    }

    OpticalProperties(InteractionType itype,
                      double trans, double refl,
                      double slope_err, double spec_err)
        : my_type(itype),
          transmitivity(trans),
          reflectivity(refl),
          slope_error(slope_err),
          specularity_error(spec_err)
    {
    }

    OpticalProperties &operator=(const OpticalProperties &rhs)
    {
        this->my_type = rhs.my_type;
        this->transmitivity = rhs.transmitivity;
        this->reflectivity = rhs.reflectivity;
        this->slope_error = rhs.slope_error;
        this->specularity_error = rhs.specularity_error;
        return *this;
    }
};

#endif
