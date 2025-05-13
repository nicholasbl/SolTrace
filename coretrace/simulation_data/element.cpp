
#include "element.hpp"

#include <math.h>

// ElementContainer ElementBase::empty_container;

ElementBase::ElementBase() : Element(),
                             active(true),
                             my_id(ELEMENT_ID_UNASSIGNED),
                             zrot(0.0)
                            //  parent(nullptr)
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
    // this->parent = nullptr;
    return;
}

// ElementContainer::iterator ElementBase::get_iterator()
// {
//     return empty_container.get_iterator();
// }

// ElementContainer::const_iterator ElementBase::get_const_iterator()
// {
//     return empty_container.get_const_iterator();
// }

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

int ElementBase::convert_reference_to_local(Vector3d &local, const Vector3d &ref)
{
    // matrix_vector_product(this->reference_to_local, ref, local);
    return 0;
}

int ElementBase::convert_global_to_local(Vector3d &local, const Vector3d &global)
{
    // No parent element means that the reference coordinate frame
    // is the global coordinate frame. Otherwise, we need to account for
    // the parent element coordinate frame when converting to global.

    int sts = 0;

    // if(this->parent == nullptr)
    // {
    //     sts = this->convert_reference_to_local(local, global);
    // }
    // else
    // {
    //     Vector3d ref;
    //     sts = this->parent->convert_reference_to_local(ref, global);
    //     // If nonzero status, something went wrong. Return that problem
    //     // regardless if that is the end of the computation or not.
    //     if (sts == 0)
    //     {
    //         sts = this->convert_reference_to_local(local, ref);
    //     }
    // }

    return sts;

}

int ElementBase::convert_local_to_reference(Vector3d &local, const Vector3d &ref)
{
    // TODO: Implement this
    return 0;
}

int ElementBase::convert_local_to_global(Vector3d &global, const Vector3d &local)
{

    // No parent element means that the reference coordinate frame
    // is the global coordinate frame. Otherwise, we need to account for
    // the parent element coordinate frame when converting to global.

    int sts = 0;

    // if (this->parent == nullptr)
    // {
    //     sts = this->convert_local_to_reference(global, local);
    // }
    // else
    // {
    //     Vector3d ref;
    //     sts = this->convert_local_to_reference(ref, local);
    //     // If nonzero status, something went wrong. Return that problem
    //     // regardless if that is the end of the computation or not.
    //     if (sts == 0)
    //     {
    //         sts = this->parent->convert_local_to_reference(global, ref);
    //     }
    // }

    return sts;

}

// const OpticalProperties & ElementBase::get_optical_properties() const
// {
//     return this->optics;
// }

// void ElementBase::set_optical_properties(const OpticalProperties &op)
// {
//     this->optics = op;
// }
