#include <gtest/gtest.h>

#include <element.hpp>
#include <plane_reflector.hpp>
#include <vector3d.hpp>

#include "common.hpp"

TEST(Element, Interface)
{
    PlaneReflector ref;
    Vector3d zero = Vector3d(0.0, 0.0, 0.0);
    EXPECT_TRUE(is_identical(ref.get_origin(), zero));
    EXPECT_TRUE(is_identical(ref.get_aim_vector(), zero));

    auto pos = Vector3d(2.0, 1.0, -3.0);
    auto aim = Vector3d(-1.0, 0.0, 1.0);

    ref.set_origin(pos);
    ref.set_aim_vector(aim);

    EXPECT_TRUE(is_identical(ref.get_origin(), pos));
    EXPECT_TRUE(is_identical(ref.get_aim_vector(), aim));

}
