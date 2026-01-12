#include <gtest/gtest.h>

#include <cmath>

#include <utilities.hpp>
#include <vector3d.hpp>

#include "common.hpp"

TEST(Utilities, Projection)
{
    Vector3d v(1.0, 1.0, 1.0);
    Vector3d u(5.0, -2.0, -3.0);

    Vector3d u_para;
    Vector3d u_perp;
    Vector3d scratch, temp1, temp2;

    project_onto_vector(v, u, u_para);
    project_onto_plane(v, u, u_perp);

    EXPECT_DOUBLE_EQ(dot_product(v, u_perp), 0.0);
    EXPECT_DOUBLE_EQ(dot_product(u_para, u_perp), 0.0);

    temp1 = u;
    temp2 = u;

    // Inplace project operations
    project_onto_vector(v, temp1);
    project_onto_plane(v, temp2);

    vector_add(1.0, u_para, 1.0, u_perp, scratch);
    for(unsigned k=0; k < 3; ++k)
    {
        EXPECT_DOUBLE_EQ(scratch[k], u[k]);
        EXPECT_DOUBLE_EQ(temp1[k], u_para[k]);
        EXPECT_DOUBLE_EQ(temp2[k], u_perp[k]);
    }
}
