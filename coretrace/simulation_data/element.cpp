
#include "element.hpp"

#include <math.h>

ElementBase::ElementBase()
{
    return;
}

ElementBase::ElementBase(const Vector3d &origin,
                         const Vector3d &aim)
    : origin(origin), aim(aim)
{
    return;
}

ElementBase::~ElementBase()
{
    return;
}

int ElementBase::compute_coordinate_rotations()
{
    int sts = 0;

    Vector3d dr;
    // TODO: Make sure this is the correct thing to do here
    vector_add(1.0, this->aim, -1.0, this->origin, dr);
    make_unit_vector(dr);

    this->euler_angles.set_value(0, atan2(dr[0], dr[2]));
    this->euler_angles.set_value(1, asin(dr[1]));
    this->euler_angles.set_value(2, this->zrot);

    compute_transform_matrices(this->euler_angles,
                               this->reference_to_local,
                               this->local_to_reference);

    return sts;
}

int ElementBase::convert_reference_to_local(Vector3d &ref, const Vector3d &local)
{
    return 0;
}

int ElementBase::convert_local_to_reference(Vector3d &local, const Vector3d &ref)
{
    return 0;
}

// const OpticalProperties & ElementBase::get_optical_properties() const
// {
//     return this->optics;
// }

// void ElementBase::set_optical_properties(const OpticalProperties &op)
// {
//     this->optics = op;
// }
