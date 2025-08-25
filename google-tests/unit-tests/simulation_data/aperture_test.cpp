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
}

TEST(Aperture, Annulus)
{
    const double RO = 5.0;
    const double RI = 1.0;
    const double ARC1 = 90.0;
    const double ARC2 = 360.0;

    // Inside both
    const double X1 = 2.0;
    const double Y1 = 1.0;
    // Outside both in center
    const double X2 = 0.5;
    const double Y2 = 0.5;
    // Outside both
    const double X3 = 1.0;
    const double Y3 = -5.0;
    // Inside ann2 but outside ann1
    const double X4 = -2.0;
    const double Y4 = -3.0;
    // Inside ann2 but outside ann1
    const double X5 = -2.0;
    const double Y5 = 3.0;
    // Inside both
    const double X6 = 2.0;
    const double Y6 = -1.0;

    auto ann1 = make_aperture<Annulus>(RI, RO, ARC1);
    auto ann2 = make_aperture<Annulus>(RI, RO, ARC2);

    EXPECT_EQ(ann1->diameter_circumscribed_circle(), 2 * RO);
    EXPECT_EQ(ann2->diameter_circumscribed_circle(), 2 * RO);

    EXPECT_EQ(ann1->radius_circumscribed_circle(), RO);
    EXPECT_EQ(ann2->radius_circumscribed_circle(), RO);

    EXPECT_EQ(ann2->aperture_area(), M_PI * (RO * RO - RI * RI));
    EXPECT_EQ(ann1->aperture_area(), 0.25 * ann2->aperture_area());

    EXPECT_TRUE(ann1->is_in(X1, Y1));
    EXPECT_FALSE(ann1->is_in(X2, Y2));
    EXPECT_FALSE(ann1->is_in(X3, Y3));
    EXPECT_FALSE(ann1->is_in(X4, Y4));
    EXPECT_FALSE(ann1->is_in(X5, Y5));
    EXPECT_TRUE(ann1->is_in(X6, Y6));

    EXPECT_TRUE(ann2->is_in(X1, Y1));
    EXPECT_FALSE(ann2->is_in(X2, Y2));
    EXPECT_FALSE(ann2->is_in(X3, Y3));
    EXPECT_TRUE(ann2->is_in(X4, Y4));
    EXPECT_TRUE(ann2->is_in(X5, Y5));
    EXPECT_TRUE(ann2->is_in(X6, Y6));

    aperture_ptr a1 = ann1->make_copy();
    EXPECT_EQ(a1->diameter_circumscribed_circle(),
              ann1->diameter_circumscribed_circle());
    EXPECT_EQ(a1->radius_circumscribed_circle(),
              ann1->radius_circumscribed_circle());
    EXPECT_EQ(a1->aperture_area(), ann1->aperture_area());
    EXPECT_TRUE(a1->is_in(X1, Y1));
    EXPECT_FALSE(a1->is_in(X2, Y2));
    EXPECT_FALSE(a1->is_in(X3, Y3));
    EXPECT_FALSE(a1->is_in(X4, Y4));
    EXPECT_FALSE(a1->is_in(X5, Y5));
}

TEST(Aperture, Circle)
{
    const double D = 2.0;
    const double X1 = 0.5;
    const double Y1 = -0.5;
    const double X2 = 1.0;
    const double Y2 = 1.5;
    auto cir = make_aperture<Circle>(D);

    EXPECT_EQ(cir->diameter_circumscribed_circle(), D);
    EXPECT_EQ(cir->radius_circumscribed_circle(), 0.5 * D);
    EXPECT_EQ(cir->aperture_area(), M_PI * 0.25 * D * D);

    EXPECT_TRUE(cir->is_in(X1, Y1));
    EXPECT_FALSE(cir->is_in(X2, Y2));

    aperture_ptr ap = cir->make_copy();
    EXPECT_EQ(ap->diameter_circumscribed_circle(),
              cir->diameter_circumscribed_circle());
    EXPECT_EQ(ap->radius_circumscribed_circle(),
              cir->radius_circumscribed_circle());
    EXPECT_EQ(ap->aperture_area(), cir->aperture_area());
    EXPECT_TRUE(ap->is_in(X1, Y1));
    EXPECT_FALSE(ap->is_in(X2, Y2));
}

TEST(Aperture, EqualateralTriangle)
{
    const double TOL = 1e-12;
    const double D = 2.0;
    const double R = 0.5 * D;
    const double S = sqrt(3.0) * R; // Side length of triangle
    const double AREA = sqrt(27.0) * R * R / 4.0;

    // Inside inscribed circle
    const double X1 = 0.1;
    const double Y1 = 0.1;
    // Inside but outside inscribed circle on left
    const double X2 = -0.375 * S;
    const double Y2 = -0.375 * R;
    // Outside circumscribed circle
    const double X3 = 1.0;
    const double Y3 = -1.0;
    // Outside but inside circumscribed circle
    const double X4 = -R / sqrt(3.0) - 0.1;
    const double Y4 = 0.1;
    // Inside but outside inscribed circle on right
    const double X5 = 0.375 * S;
    const double Y5 = -0.375 * R;

    auto et = make_aperture<EqualateralTriangle>(D);

    EXPECT_EQ(et->diameter_circumscribed_circle(), D);
    EXPECT_EQ(et->radius_circumscribed_circle(), 0.5 * D);
    EXPECT_NEAR(et->aperture_area(), AREA, TOL);

    EXPECT_TRUE(et->is_in(X1, Y1));
    EXPECT_TRUE(et->is_in(X2, Y2));
    EXPECT_FALSE(et->is_in(X3, Y3));
    EXPECT_FALSE(et->is_in(X4, Y4));
    EXPECT_TRUE(et->is_in(X5, Y5));

    aperture_ptr ap = et->make_copy();
    EXPECT_EQ(ap->diameter_circumscribed_circle(),
              et->diameter_circumscribed_circle());
    EXPECT_EQ(ap->radius_circumscribed_circle(),
              et->radius_circumscribed_circle());
    EXPECT_EQ(ap->aperture_area(), et->aperture_area());
    EXPECT_TRUE(ap->is_in(X1, Y1));
    EXPECT_FALSE(ap->is_in(X3, Y3));
}

TEST(Aperture, Hexagon)
{
    const double TOL = 1e-12;
    const double D = 2.0;
    const double R = 0.5 * D;
    const double S = sqrt(3.0) * R; // Side length of hexagon
    const double AREA = 0.5 * sqrt(27.0) * R * R;

    const double X1 = 1.0;
    const double Y1 = 1.0;
    const double X2 = -0.5;
    const double Y2 = 0.5;
    const double X3 = 0.9;
    const double Y3 = 0.1;
    const double X4 = 0.9;
    const double Y4 = 0.25;
    const double X5 = -0.45;
    const double Y5 = -0.8;
    const double X6 = 0.1;
    const double Y6 = 0.95;

    auto hex = make_aperture<Hexagon>(D);

    EXPECT_EQ(hex->diameter_circumscribed_circle(), D);
    EXPECT_EQ(hex->radius_circumscribed_circle(), R);
    EXPECT_NEAR(hex->aperture_area(), AREA, TOL);

    // Outside Circumscribed, Inside Inscribed
    EXPECT_FALSE(hex->is_in(X1, Y1));
    EXPECT_TRUE(hex->is_in(X2, Y2));
    // Left side inside (outside inscribed circle), outside above, below
    EXPECT_TRUE(hex->is_in(-X3, Y3));
    EXPECT_FALSE(hex->is_in(-X4, Y4));
    EXPECT_FALSE(hex->is_in(-X4, -Y4));
    // Right side inside (outside inscribed circle), outside above, below
    EXPECT_TRUE(hex->is_in(X3, Y3));
    EXPECT_FALSE(hex->is_in(X4, Y4));
    EXPECT_FALSE(hex->is_in(X4, -Y4));
    // Center inside (outside inscribed circle), outside above, below
    EXPECT_TRUE(hex->is_in(X5, Y5));
    EXPECT_FALSE(hex->is_in(X6, Y6));
    EXPECT_FALSE(hex->is_in(-X6, -Y6));

    aperture_ptr ap = hex->make_copy();
    EXPECT_EQ(ap->diameter_circumscribed_circle(),
              hex->diameter_circumscribed_circle());
    EXPECT_EQ(ap->radius_circumscribed_circle(),
              hex->radius_circumscribed_circle());
    EXPECT_EQ(ap->aperture_area(), hex->aperture_area());
    EXPECT_TRUE(ap->is_in(X2, Y2));
    EXPECT_FALSE(ap->is_in(X4, Y4));
}

TEST(Aperture, Rectangle)
{
    /**** Common Constants ****/
    const double TOL = 1e-12;
    const double D = 2.0;
    const double LY = 1.0;
    const double LX = sqrt(D * D - LY * LY);
    const double AREA = LY * LX;

    /**** Rectangle that is at the origin ****/
    // Inside
    const double X1 = -0.5 * LX;
    const double Y1 = 0.5 * LY;
    // Outside left
    const double X2 = -2.0 * LX;
    const double Y2 = Y1;
    // Outside right
    const double X3 = 2.0 * LX;
    const double Y3 = -Y1;
    // Outside top
    const double X4 = X1;
    const double Y4 = 1.5 * LY;
    // Outside bottom
    const double X5 = -X1;
    const double Y5 = -1.5 * LY;

    auto rect = make_aperture<Rectangle>(LX, LY);

    EXPECT_NEAR(rect->diameter_circumscribed_circle(), D, TOL);
    EXPECT_NEAR(rect->radius_circumscribed_circle(), 0.5 * D, TOL);
    EXPECT_NEAR(rect->aperture_area(), AREA, TOL);

    EXPECT_TRUE(rect->is_in(X1, Y1));
    EXPECT_FALSE(rect->is_in(X2, Y2));
    EXPECT_FALSE(rect->is_in(X3, Y3));
    EXPECT_FALSE(rect->is_in(X4, Y4));
    EXPECT_FALSE(rect->is_in(X5, Y5));

    aperture_ptr ap = rect->make_copy();
    EXPECT_EQ(ap->diameter_circumscribed_circle(),
              rect->diameter_circumscribed_circle());
    EXPECT_EQ(ap->radius_circumscribed_circle(),
              rect->radius_circumscribed_circle());
    EXPECT_EQ(ap->aperture_area(), rect->aperture_area());
    EXPECT_TRUE(ap->is_in(X1, Y1));
    EXPECT_FALSE(ap->is_in(X3, Y3));

    /**** Rectangle that is shifted from the origin ****/
    const double XL = -1.0;
    const double YL = -2.0;

    // Inside
    const double X6 = 0.5 * LX + XL;
    const double Y6 = 0.5 * LY + YL;
    // Outside left
    const double X7 = -2.0 * LX + XL;
    const double Y7 = Y1 + YL;
    // Outside right
    const double X8 = 2.0 * LX + XL;
    const double Y8 = -Y1 + YL;
    // Outside top
    const double X9 = X1 + XL;
    const double Y9 = 1.5 * LY + YL;
    // Outside bottom
    const double X10 = -X1 + XL;
    const double Y10 = -1.5 * LY + YL;

    auto rect_shift = make_aperture<Rectangle>(LX, LY, XL, YL);

    EXPECT_NEAR(rect_shift->diameter_circumscribed_circle(), D, TOL);
    EXPECT_NEAR(rect_shift->radius_circumscribed_circle(), 0.5 * D, TOL);
    EXPECT_NEAR(rect_shift->aperture_area(), AREA, TOL);

    EXPECT_TRUE(rect_shift->is_in(X6, Y6));
    EXPECT_FALSE(rect_shift->is_in(X7, Y7));
    EXPECT_FALSE(rect_shift->is_in(X8, Y8));
    EXPECT_FALSE(rect_shift->is_in(X9, Y9));
    EXPECT_FALSE(rect_shift->is_in(X10, Y10));

    aperture_ptr ap_shift = rect_shift->make_copy();
    EXPECT_EQ(ap_shift->diameter_circumscribed_circle(),
              rect_shift->diameter_circumscribed_circle());
    EXPECT_EQ(ap_shift->radius_circumscribed_circle(),
              rect_shift->radius_circumscribed_circle());
    EXPECT_EQ(ap_shift->aperture_area(), rect_shift->aperture_area());
    EXPECT_TRUE(ap_shift->is_in(X6, Y6));
    EXPECT_FALSE(ap_shift->is_in(X8, Y8));
}

TEST(Aperture, IrregularTriangle)
{
    const double TOL = 1e-12;
    const double x1 = 0.0, x2 = 1.0, x3 = 2.0 * x2;
    const double y1 = 0.0, y2 = 2.0, y3 = y1;
    auto tri = make_aperture<IrregularTriangle>(x1, y1, x2, y2, x3, y3);

    EXPECT_NEAR(tri->aperture_area(), 0.5 * y2 * (x3 - x1), TOL);

    EXPECT_TRUE(tri->is_in(1.0, 1.0));
    EXPECT_FALSE(tri->is_in(1.5, 2.0));

    auto ap = tri->make_copy();
    EXPECT_NEAR(ap->aperture_area(), 0.5 * y2 * (x3 - x1), TOL);
    EXPECT_TRUE(ap->is_in(1.0, 1.0));
    EXPECT_FALSE(ap->is_in(1.5, 2.0));
}

TEST(Aperture, IrregularQuadrilateral)
{
    const double TOL = 1e-12;
    // Parallelogram
    const double x1 = 0.0, x2 = 3.0, x3 = (x2 - x1) + 1.0, x4 = x3 - x2 + x1;
    const double y1 = 0.0, y2 = y1, y3 = 2.0, y4 = y3;
    auto quad = make_aperture<IrregularQuadrilateral>(
        x1, y1, x2, y2, x3, y3, x4, y4);

    // EXPECT_NEAR(quad->aperture_area(), (y2 - y1) * (x3 - x1), TOL);
    EXPECT_NEAR(quad->aperture_area(), (x3 - x4) * (y3 - y1), TOL);

    EXPECT_TRUE(quad->is_in(3.0, 1.0));
    EXPECT_TRUE(quad->is_in(1.0, 1.5));
    EXPECT_FALSE(quad->is_in(4.0, 1.0));

    auto ap = quad->make_copy();
    EXPECT_NEAR(ap->aperture_area(), (x3 - x4) * (y3 - y1), TOL);
    EXPECT_TRUE(ap->is_in(3.0, 1.0));
    EXPECT_TRUE(ap->is_in(1.0, 1.5));
    EXPECT_FALSE(ap->is_in(4.0, 1.0));
}
