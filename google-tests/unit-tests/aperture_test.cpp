#include <gtest/gtest.h>

#include <aperture.hpp>

#include "common.hpp"

TEST(Aperture, ApertureBase)
{
    struct TestAperture : public Aperture
    {
        double my_value;
        TestAperture(double mv, ApertureType at) : Aperture(at), my_value(mv)
        {
        }
        virtual ~TestAperture() {}
        virtual double aperture_area() const { return my_value; }
        virtual double diameter_circumscribed_circle() const { return 2.0; }
        virtual bool is_in(double x, double y) const { return false; }
        virtual aperture_ptr make_copy() const
        {
            return make_aperture<TestAperture>(*this);
        }
        // virtual TestAperture& operator=(const TestAperture &rhs)
        // {
        //     Aperture::operator=(rhs);
        //     this->my_value = rhs.my_value;
        //     return *this;
        // }
    };

    TestAperture ta1(1.2, CIRCLE);
    TestAperture ta2(5.3, RECTANGLE);

    ta1 = ta2;
    EXPECT_EQ(ta1.my_type, ta2.my_type);
    EXPECT_EQ(ta1.my_value, ta2.my_value);

    EXPECT_EQ(ta1.radius_circumscribed_circle(), 1.0);

    auto ta3 = ta1.make_copy();
    EXPECT_EQ(ta3->aperture_area(), ta1.aperture_area());
    EXPECT_EQ(ta3->get_type(), ta1.get_type());

    // aperture_ptr ap1 = make_aperture<TestAperture>(-21.3, HEXAGON);
    // auto ap2 = make_aperture<TestAperture>(*ap1);
    // EXPECT_EQ(ap2->my_value, -21.3);
}
