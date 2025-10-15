
#include "common.hpp"

#include <cmath>

#include <aperture.hpp>
#include <element.hpp>
#include <single_element.hpp>
#include <surface.hpp>
#include <vector3d.hpp>

bool is_identical(const Vector3d &x, const Vector3d &y)
{
    return (
        x.data[0] == y.data[0] &&
        x.data[1] == y.data[1] &&
        x.data[2] == y.data[2]);
}

bool is_identical(const Vector3d &x, const Vector3d &y, double tol)
{
    return (
        fabs(x.data[0] - y.data[0]) <= tol &&
        fabs(x.data[1] - y.data[1]) <= tol &&
        fabs(x.data[2] - y.data[2]) <= tol);
}

bool is_identical(const Matrix3d &A, const Matrix3d &B)
{
    bool all_identical = true;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            all_identical &= A.data[i][j] == B.data[i][j];
        }
    }
    return all_identical;
}

element_ptr make_configured_element()
{
    element_ptr el = SolTrace::Data::make_element<SolTrace::Data::SingleElement>();
    el->set_aperture(SolTrace::Data::make_aperture<SolTrace::Data::Circle>(2.0));
    el->set_surface(SolTrace::Data::make_surface<SolTrace::Data::Flat>());
    return el;
}
