#include <gtest/gtest.h>

#include <surface.hpp>

TEST(Surface, Typing)
{
    // Test that each constructor properly sets the type
    auto cone = SolTrace::Data::make_surface<SolTrace::Data::Cone>(50.0);
    EXPECT_EQ(cone->get_type(), SolTrace::Data::CONE);
    // cone = SolTrace::Data::make_surface<Cone>();
    // EXPECT_EQ(cone->get_type(), CONE);

    auto cylinder = SolTrace::Data::make_surface<SolTrace::Data::Cylinder>(1.0);
    EXPECT_EQ(cylinder->get_type(), SolTrace::Data::CYLINDER);

    auto flat = SolTrace::Data::make_surface<SolTrace::Data::Flat>();
    EXPECT_EQ(flat->get_type(), SolTrace::Data::FLAT);

    auto para = SolTrace::Data::make_surface<SolTrace::Data::Parabola>(1.0, 1.0);
    EXPECT_EQ(para->get_type(), SolTrace::Data::PARABOLA);
    // para = SolTrace::Data::make_surface<Parabola>();
    // EXPECT_EQ(para->get_type(), PARABOLA);

    auto sph = SolTrace::Data::make_surface<SolTrace::Data::Sphere>(10.0);
    EXPECT_EQ(sph->get_type(), SolTrace::Data::SPHERE);
    // sph = SolTrace::Data::make_surface<Sphere>();
    // EXPECT_EQ(sph->get_type(), SPHERE);
}

TEST(Surface, MakeSurfaceFromType)
{
    // Test CONE creation with valid arguments
    {
        std::vector<double> args = {45.0};
        auto surface = SolTrace::Data::make_surface_from_type(SolTrace::Data::CONE, args);
        ASSERT_NE(surface, nullptr);
        EXPECT_EQ(surface->get_type(), SolTrace::Data::CONE);
        
        auto cone = std::dynamic_pointer_cast<SolTrace::Data::Cone>(surface);
        ASSERT_NE(cone, nullptr);
        EXPECT_DOUBLE_EQ(cone->half_angle, 45.0);
    }

    // Test CONE creation with insufficient arguments
    {
        std::vector<double> args; // Empty args
        auto surface = SolTrace::Data::make_surface_from_type(SolTrace::Data::CONE, args);
        EXPECT_EQ(surface, nullptr);
    }

    // Test CYLINDER creation with valid arguments (note: args[0] is inverted to get radius)
    {
        std::vector<double> args = {0.5}; // 1/0.5 = 2.0 radius
        auto surface = SolTrace::Data::make_surface_from_type(SolTrace::Data::CYLINDER, args);
        ASSERT_NE(surface, nullptr);
        EXPECT_EQ(surface->get_type(), SolTrace::Data::CYLINDER);
        
        auto cylinder = std::dynamic_pointer_cast<SolTrace::Data::Cylinder>(surface);
        ASSERT_NE(cylinder, nullptr);
        EXPECT_DOUBLE_EQ(cylinder->radius, 2.0);
    }

    // Test CYLINDER creation with insufficient arguments
    {
        std::vector<double> args; // Empty args
        auto surface = SolTrace::Data::make_surface_from_type(SolTrace::Data::CYLINDER, args);
        EXPECT_EQ(surface, nullptr);
    }

    // Test FLAT creation (no parameters needed)
    {
        std::vector<double> args; // Empty args for flat surface
        auto surface = SolTrace::Data::make_surface_from_type(SolTrace::Data::FLAT, args);
        ASSERT_NE(surface, nullptr);
        EXPECT_EQ(surface->get_type(), SolTrace::Data::FLAT);
    }

    // Test PARABOLA creation with valid arguments (requires 2 parameters)
    {
        std::vector<double> args = {1.5, 2.0};
        auto surface = SolTrace::Data::make_surface_from_type(SolTrace::Data::PARABOLA, args);
        ASSERT_NE(surface, nullptr);
        EXPECT_EQ(surface->get_type(), SolTrace::Data::PARABOLA);
        
        auto parabola = std::dynamic_pointer_cast<SolTrace::Data::Parabola>(surface);
        ASSERT_NE(parabola, nullptr);
        EXPECT_DOUBLE_EQ(parabola->focal_length_x, 1.5);
        EXPECT_DOUBLE_EQ(parabola->focal_length_y, 2.0);
    }

    // Test PARABOLA creation with insufficient arguments
    {
        std::vector<double> args = {1.5}; // Only 1 argument, needs 2
        auto surface = SolTrace::Data::make_surface_from_type(SolTrace::Data::PARABOLA, args);
        EXPECT_EQ(surface, nullptr);
        
        std::vector<double> empty_args; // No arguments
        auto surface2 = SolTrace::Data::make_surface_from_type(SolTrace::Data::PARABOLA, empty_args);
        EXPECT_EQ(surface2, nullptr);
    }

    // Test SPHERE creation with valid arguments
    {
        std::vector<double> args = {0.1}; // vertex curvature
        auto surface = SolTrace::Data::make_surface_from_type(SolTrace::Data::SPHERE, args);
        ASSERT_NE(surface, nullptr);
        EXPECT_EQ(surface->get_type(), SolTrace::Data::SPHERE);
        
        auto sphere = std::dynamic_pointer_cast<SolTrace::Data::Sphere>(surface);
        ASSERT_NE(sphere, nullptr);
        EXPECT_DOUBLE_EQ(sphere->vertex_curv, 0.1);
    }

    // Test SPHERE creation with insufficient arguments
    {
        std::vector<double> args; // Empty args
        auto surface = SolTrace::Data::make_surface_from_type(SolTrace::Data::SPHERE, args);
        EXPECT_EQ(surface, nullptr);
    }

    // Test unimplemented surface types (should return nullptr)
    {
        std::vector<double> args = {1.0, 2.0};
        
        auto hyper = SolTrace::Data::make_surface_from_type(SolTrace::Data::HYPER, args);
        EXPECT_EQ(hyper, nullptr);
        
        auto spencer_murty = SolTrace::Data::make_surface_from_type(SolTrace::Data::GENERAL_SPENCER_MURTY, args);
        EXPECT_EQ(spencer_murty, nullptr);
        
        auto torus = SolTrace::Data::make_surface_from_type(SolTrace::Data::TORUS, args);
        EXPECT_EQ(torus, nullptr);
        
        auto unknown = SolTrace::Data::make_surface_from_type(SolTrace::Data::SURFACE_UNKNOWN, args);
        EXPECT_EQ(unknown, nullptr);
    }

    // Test with various argument vectors to ensure robustness
    {
        // Test that extra arguments don't cause issues (should be ignored)
        std::vector<double> multi_args = {5.0, 7.5, 10.0, 12.5}; // Extra args should be ignored
        
        // CONE only uses first argument
        auto cone = SolTrace::Data::make_surface_from_type(SolTrace::Data::CONE, multi_args);
        ASSERT_NE(cone, nullptr);
        EXPECT_EQ(cone->get_type(), SolTrace::Data::CONE);
        auto cone_cast = std::dynamic_pointer_cast<SolTrace::Data::Cone>(cone);
        EXPECT_DOUBLE_EQ(cone_cast->half_angle, 5.0);
        
        // CYLINDER only uses first argument
        auto cylinder = SolTrace::Data::make_surface_from_type(SolTrace::Data::CYLINDER, multi_args);
        ASSERT_NE(cylinder, nullptr);
        EXPECT_EQ(cylinder->get_type(), SolTrace::Data::CYLINDER);
        auto cylinder_cast = std::dynamic_pointer_cast<SolTrace::Data::Cylinder>(cylinder);
        EXPECT_DOUBLE_EQ(cylinder_cast->radius, 1.0/5.0);
        
        // PARABOLA uses first two arguments
        auto parabola = SolTrace::Data::make_surface_from_type(SolTrace::Data::PARABOLA, multi_args);
        ASSERT_NE(parabola, nullptr);
        EXPECT_EQ(parabola->get_type(), SolTrace::Data::PARABOLA);
        auto para_cast = std::dynamic_pointer_cast<SolTrace::Data::Parabola>(parabola);
        EXPECT_DOUBLE_EQ(para_cast->focal_length_x, 5.0);
        EXPECT_DOUBLE_EQ(para_cast->focal_length_y, 7.5);
        
        // SPHERE only uses first argument
        auto sphere = SolTrace::Data::make_surface_from_type(SolTrace::Data::SPHERE, multi_args);
        ASSERT_NE(sphere, nullptr);
        EXPECT_EQ(sphere->get_type(), SolTrace::Data::SPHERE);
        auto sphere_cast = std::dynamic_pointer_cast<SolTrace::Data::Sphere>(sphere);
        EXPECT_DOUBLE_EQ(sphere_cast->vertex_curv, 5.0);
    }
}
