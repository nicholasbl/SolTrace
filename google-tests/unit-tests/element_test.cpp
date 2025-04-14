#include <gtest/gtest.h>

#include <composite_element.hpp>
#include <element.hpp>
#include <single_element.hpp>
#include <vector3d.hpp>

#include "common.hpp"

TEST(Element, Construction)
{
    auto elem = make_element<SingleElement>();
    auto comp = make_element<CompositeElement>();
}

TEST(Element, SingleElementAccessors)
{
    SingleElement ref;
    Vector3d zero = Vector3d(0.0, 0.0, 0.0);
    EXPECT_TRUE(is_identical(ref.get_origin(), zero));
    EXPECT_TRUE(is_identical(ref.get_aim_vector(), zero));
    EXPECT_TRUE(is_identical(ref.get_euler_angles(), zero));
    EXPECT_EQ(ref.get_aperature(), nullptr);
    EXPECT_EQ(ref.get_surface(), nullptr);

    EXPECT_TRUE(ref.is_enabled());
    ref.disable();
    EXPECT_FALSE(ref.is_enabled());
    ref.enable();
    EXPECT_TRUE(ref.is_enabled());

    auto pos = Vector3d(2.0, 1.0, -3.0);
    ref.set_origin(pos);
    EXPECT_TRUE(is_identical(ref.get_origin(), pos));

    auto aim = Vector3d(-1.0, 0.0, 1.0);
    ref.set_aim_vector(aim);
    EXPECT_TRUE(is_identical(ref.get_aim_vector(), aim));

    auto eulers = Vector3d(0.1, 0.2, -0.3);
    ref.set_euler_angles(eulers);
    EXPECT_TRUE(is_identical(ref.get_euler_angles(), eulers));

    const double ZROT = 0.58;
    ref.set_zrot(ZROT);
    EXPECT_EQ(ref.get_zrot(), ZROT);

    const double D = 1.0;
    auto ap = make_aperature<Circular>(D);
    ref.set_aperature(ap);
    auto rap = std::dynamic_pointer_cast<Circular>(ref.get_aperature());
    EXPECT_FALSE(rap == nullptr);
    EXPECT_EQ(rap->diameter, D);

    const double HA = 0.25;
    auto sp = make_surface<Conical>(HA);
    ref.set_surface(sp);
    auto rsp = std::dynamic_pointer_cast<Conical>(ref.get_surface());
    EXPECT_FALSE(rsp == nullptr);
    EXPECT_EQ(rsp->half_angle, HA);

    // TODO: Optical properties
}
