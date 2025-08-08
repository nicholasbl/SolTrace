#ifndef SOLTRACE_SUN_H
#define SOLTRACE_SUN_H

#include "ray_source.hpp"

#include "error_distributions.hpp"
#include "vector3d.hpp"

class Sun : public RaySource
{
public:
    Sun() { this->my_position.zero(); }
    virtual ~Sun() {}

    virtual const Vector3d &get_position() const
    {
        return this->my_position;
    }
    virtual Vector3d &get_position()
    {
        return this->my_position;
    }
    virtual void set_position(const Vector3d &pos)
    {
        this->my_position = pos;
        return;
    }
    virtual void set_position(double x, double y, double z)
    {
        this->my_position.set_values(x, y, z);
        return;
    }
    virtual void set_position(const DateTime &, double lat, double long) {}
    virtual DistributionType get_shape() const
    {
        return this->my_shape;
    }
    virtual void set_shape(DistributionType shape, double _sigma, double _half_width,
        std::vector<double> _user_angle = {}, std::vector<double> _user_intensity = {})
    {
        this->my_shape = shape;

        // Clear arguments
        sigma = std::numeric_limits<double>::quiet_NaN();
        half_width = std::numeric_limits<double>::quiet_NaN();
        user_angle.clear();
        user_intensity.clear();

        switch (shape)
        {
            case(DistributionType::GAUSSIAN):
                sigma = _sigma;
                break;
            case(DistributionType::PILLBOX):
				half_width = _half_width;
				break;
            case(DistributionType::USER_DEFINED):
                user_angle = std::move(_user_angle);
                user_intensity = std::move(_user_intensity);
                break;
            default:
                // TODO throw error
                break;
        }

        return;
    }

private:
    DistributionType my_shape;
    Vector3d my_position;
};

#endif
