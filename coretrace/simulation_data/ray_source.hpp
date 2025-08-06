#ifndef SOLTRACE_RAY_SOURCE_H
#define SOLTRACE_RAY_SOURCE_H

#include <map>

#include "container.hpp"
#include "datetime.hpp"
#include "error_distributions.hpp"
#include "vector3d.hpp"

class RaySource
{
public:
    RaySource() {}
    virtual ~RaySource() {}

    virtual const Vector3d &get_position() const = 0;
    virtual Vector3d &get_position() = 0;
    virtual void set_position(const Vector3d &) = 0;
    virtual void set_position(double, double, double) = 0;
    virtual void set_position(const DateTime &, double lat, double long) = 0;
    virtual DistributionType get_shape() const = 0;
    virtual void set_shape(DistributionType shape) = 0;

private:
};

using ray_source_id = std::int_fast64_t;
// using ray_source_ptr = typename std::shared_ptr<RaySource>;
// using RaySourceContainer = typename std::map<ray_source_id, ray_source_ptr>;
using RaySourceContainer = Container<ray_source_id, RaySource>;
using ray_source_ptr = RaySourceContainer::value_pointer;

template<typename C, typename... Args>
inline auto make_ray_source(Args&&... args)
{
    return RaySourceContainer::make_pointer<C>(std::forward<Args>(args)...);
}

#endif
