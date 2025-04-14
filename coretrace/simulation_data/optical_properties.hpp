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

    bool operator==(const OpticalProperties &rhs) const
    {
        return !(*this != rhs);
    }

    bool operator!=(const OpticalProperties &rhs) const
    {
        return (
            this->transmitivity != rhs.transmitivity ||
            this->reflectivity != rhs.reflectivity ||
            this->slope_error != rhs.slope_error ||
            this->specularity_error == rhs.specularity_error
        );
    }
};

// bool operator==(const OpticalProperties &lhs, const OpticalProperties &rhs)
// {
//     return (
//         lhs.transmitivity == rhs.transmitivity &&
//         lhs.reflectivity == rhs.reflectivity &&
//         lhs.slope_error == rhs.slope_error &&
//         lhs.specularity_error == rhs.specularity_error
//     );
// }

#endif
