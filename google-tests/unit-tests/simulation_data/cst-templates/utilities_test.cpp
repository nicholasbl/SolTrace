#include <gtest/gtest.h>

#include <cmath>

#include <cst_templates/utilities.hpp>

#include "common.hpp"

TEST(Utilities, Projection)
{
    glm::dvec3 v(1.0, 1.0, 1.0);
    glm::dvec3 u(5.0, -2.0, -3.0);

    glm::dvec3 u_para;
    glm::dvec3 u_perp;
    glm::dvec3 scratch, temp1, temp2;

    SolTrace::Data::project_onto_vector(v, u, u_para);
    SolTrace::Data::project_onto_plane(v, u, u_perp);

    EXPECT_DOUBLE_EQ(glm::dot(v, u_perp), 0.0);
    EXPECT_DOUBLE_EQ(glm::dot(u_para, u_perp), 0.0);

    temp1 = u;
    temp2 = u;

    // Inplace project operations
    SolTrace::Data::project_onto_vector(v, temp1);
    SolTrace::Data::project_onto_plane(v, temp2);

    scratch = u_para + u_perp;
    for(unsigned k=0; k < 3; ++k)
    {
        EXPECT_DOUBLE_EQ(scratch[k], u[k]);
        EXPECT_DOUBLE_EQ(temp1[k], u_para[k]);
        EXPECT_DOUBLE_EQ(temp2[k], u_perp[k]);
    }
}
