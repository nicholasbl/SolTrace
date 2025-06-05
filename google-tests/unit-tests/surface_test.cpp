#include <gtest/gtest.h>

#include <surface.hpp>

TEST(Surface, Typing)
{
    // Test that each constructor properly sets the type
    auto cone = make_surface<Cone>(50.0);
    EXPECT_EQ(cone->get_type(), CONE);
    // cone = make_surface<Cone>();
    // EXPECT_EQ(cone->get_type(), CONE);

    auto cylinder = make_surface<Cylinder>();
    EXPECT_EQ(cylinder->get_type(), CYLINDER);

    auto flat = make_surface<Flat>();
    EXPECT_EQ(flat->get_type(), FLAT);

    auto para = make_surface<Parabola>(1.0, 1.0);
    EXPECT_EQ(para->get_type(), PARABOLA);
    // para = make_surface<Parabola>();
    // EXPECT_EQ(para->get_type(), PARABOLA);

    auto sph = make_surface<Sphere>(10.0);
    EXPECT_EQ(sph->get_type(), SPHERE);
    // sph = make_surface<Sphere>();
    // EXPECT_EQ(sph->get_type(), SPHERE);
}
