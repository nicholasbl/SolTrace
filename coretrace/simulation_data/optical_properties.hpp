#ifndef SOLTRACE_OPTICAL_PROPERTIES_H
#define SOLTRACE_OPTICAL_PROPERTIES_H

struct OpticalProperties
{
    double transmitivity;
    double reflectivity;
    double slope_error;
    double specularity_error;

    OpticalProperties() : transmitivity(0.0),
                          reflectivity(0.0),
                          slope_error(0.0),
                          specularity_error(0.0)
    {
    }

    OpticalProperties &operator=(const OpticalProperties &rhs)
    {
        this->transmitivity = rhs.transmitivity;
        this->reflectivity = rhs.reflectivity;
        this->slope_error = rhs.slope_error;
        this->specularity_error = rhs.specularity_error;
        return *this;
    }
};

#endif
