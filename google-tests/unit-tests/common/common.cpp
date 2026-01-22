
#include "common.hpp"

#include <cmath>

#include <aperture.hpp>
#include <element.hpp>
#include <single_element.hpp>
#include <surface.hpp>

bool is_identical(const glm::dvec3 &x, const glm::dvec3 &y)
{
    return x == y;
}

bool is_identical(const glm::dvec3 &x, const glm::dvec3 &y, double tol)
{
    return (
        fabs(x.x - y.x) <= tol &&
        fabs(x.y - y.y) <= tol &&
        fabs(x.z - y.z) <= tol);
}

bool is_identical(const glm::dmat3 &A, const glm::dmat3 &B)
{
    return A == B;
}

element_ptr make_configured_element()
{
    element_ptr el = SolTrace::Data::make_element<SolTrace::Data::SingleElement>();
    el->set_aperture(SolTrace::Data::make_aperture<SolTrace::Data::Circle>(2.0));
    el->set_surface(SolTrace::Data::make_surface<SolTrace::Data::Flat>());
    return el;
}
