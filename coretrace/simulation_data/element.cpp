
#include "element.hpp"

#include <math.h>

ElementBase::ElementBase() : Element(),
                             active(true),
                             my_id(ELEMENT_ID_UNASSIGNED),
                             zrot(0.0)
{
    this->aim.zero();
    this->origin.zero();
    this->euler_angles.zero();

    this->reference_to_local.zero();
    this->local_to_reference.zero();

    return;
}

// ElementBase::ElementBase(const Vector3d &origin,
//                          const Vector3d &aim)
//     : active(true), origin(origin), aim(aim), optics_back(), optics_front()
// {
//     this->euler_angles.zero();

//     this->reference_to_local.zero();
//     this->local_to_reference.zero();

//     return;
// }

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

    this->euler_angles[0] = atan2(dr[0], dr[2]);
    this->euler_angles[1] = asin(dr[1]);
    this->euler_angles[2] = this->zrot;

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
