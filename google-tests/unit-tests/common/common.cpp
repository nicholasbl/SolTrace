
#include "common.hpp"

#include <cmath>

#include <aperture.hpp>
#include <element.hpp>
#include <single_element.hpp>
#include <surface.hpp>

#include <glm/ext/matrix_common.hpp>
#include <glm/gtx/io.hpp>

bool is_identical(const glm::dvec3 &x, const glm::dvec3 &y)
{
    return x == y;
}

bool is_identical(const glm::dvec3 &x, const glm::dvec3 &y, double tol)
{
    bool check = (fabs(x.x - y.x) <= tol && fabs(x.y - y.y) <= tol && fabs(x.z - y.z) <= tol);

    if (!check) {
        std::cout << "Vectors not equal: A = " << x << ", B = " << y << std::endl;
    }
    return (
        fabs(x.x - y.x) <= tol &&
        fabs(x.y - y.y) <= tol &&
        fabs(x.z - y.z) <= tol);
}

bool is_identical(const glm::dmat3 &A, const glm::dmat3 &B)
{
    double maxAbs = 0.0f;
    for (int c = 0; c < 3; ++c) {
        for (int r = 0; r < 3; ++r) {
            maxAbs = std::max<double>(maxAbs, std::abs(A[c][r] - B[c][r]));
        }
    }

    bool check = maxAbs < 1E-300;

    if (!check) {
        std::cout << "Matrix not equal: A = " << A << ", B = " << B << std::endl;
        std::cout << "Tolerance = " << maxAbs << std::endl;
    }

    return check;
}

element_ptr make_configured_element()
{
    element_ptr el = SolTrace::Data::make_element<SolTrace::Data::SingleElement>();
    el->set_aperture(SolTrace::Data::make_aperture<SolTrace::Data::Circle>(2.0));
    el->set_surface(SolTrace::Data::make_surface<SolTrace::Data::Flat>());
    return el;
}
