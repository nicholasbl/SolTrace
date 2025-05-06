#include <gtest/gtest.h>

#include <composite_element.hpp>
#include <element.hpp>
#include <single_element.hpp>
#include <virtual_element.hpp>
#include <vector3d.hpp>

#include "common.hpp"

TEST(Element, Construction)
{
    // Smoke tests
    auto elem = make_element<SingleElement>();
    auto comp = make_element<CompositeElement>();
    auto ve = make_element<VirtualElement>();
    auto vp = make_element<VirtualPlane>(10.0, 10.0);
}

TEST(Element, SingleElementAccessors)
{
    SingleElement ref;
    Vector3d zero = Vector3d(0.0, 0.0, 0.0);
    EXPECT_TRUE(is_identical(ref.get_origin(), zero));
    EXPECT_TRUE(is_identical(ref.get_aim_vector(), zero));
    EXPECT_TRUE(is_identical(ref.get_euler_angles(), zero));
    EXPECT_EQ(ref.get_aperture(), nullptr);
    EXPECT_EQ(ref.get_surface(), nullptr);

    EXPECT_TRUE(ref.is_enabled());
    ref.disable();
    EXPECT_FALSE(ref.is_enabled());
    ref.enable();
    EXPECT_TRUE(ref.is_enabled());

    EXPECT_FALSE(ref.is_composite());
    EXPECT_FALSE(ref.is_virtual());
    EXPECT_TRUE(ref.is_single());

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
    auto ap = make_aperture<Circular>(D);
    ref.set_aperture(ap);
    auto rap = std::dynamic_pointer_cast<Circular>(ref.get_aperture());
    EXPECT_NE(rap, nullptr);
    EXPECT_EQ(rap->diameter, D);

    const double HA = 0.25;
    auto sp = make_surface<Conical>(HA);
    ref.set_surface(sp);
    auto rsp = std::dynamic_pointer_cast<Conical>(ref.get_surface());
    EXPECT_FALSE(rsp == nullptr);
    EXPECT_EQ(rsp->half_angle, HA);

    auto opf = ref.get_front_optical_properties();
    auto opb = ref.get_back_optical_properties();
    EXPECT_EQ(opf->transmitivity, opb->transmitivity);
    EXPECT_EQ(opf->reflectivity, opb->reflectivity);
    EXPECT_EQ(opf->slope_error, opb->slope_error);
    EXPECT_EQ(opf->specularity_error, opb->specularity_error);

    OpticalProperties op(0.75, 0.25, 0.1, 0.001);
    ref.set_front_optical_properties(op);
    // EXPECT_EQ(*opf, op);
    EXPECT_EQ(opf->transmitivity, op.transmitivity);
    EXPECT_EQ(opf->reflectivity, op.reflectivity);
    EXPECT_EQ(opf->slope_error, op.slope_error);
    EXPECT_EQ(opf->specularity_error, op.specularity_error);

    ref.set_back_optical_properties(op);
    EXPECT_EQ(opb->transmitivity, op.transmitivity);
    EXPECT_EQ(opb->reflectivity, op.reflectivity);
    EXPECT_EQ(opb->slope_error, op.slope_error);
    EXPECT_EQ(opb->specularity_error, op.specularity_error);
}

TEST(Element, SingleElementOrientationUpdate)
{
    // TODO: Implement this test.
    return;
}

TEST(Element, VirtualElement)
{
    VirtualElement ve;

    EXPECT_TRUE(ve.is_virtual());
    
    const double LX = 1.0;
    const double LY = 2.0;
    ve.set_aperture(make_aperture<Rectangular>(LX, LY));
    auto aptr = ve.get_aperture();
    EXPECT_EQ(aptr->get_type(), RECTANGULAR);
    auto rptr = std::dynamic_pointer_cast<Rectangular>(aptr);
    EXPECT_NE(rptr, nullptr);
    if(rptr != nullptr)
    {
        EXPECT_EQ(rptr->x_length, LX);
        EXPECT_EQ(rptr->y_length, LY);
    }
    
    ve.set_surface(make_surface<Flat>());
    auto sptr = ve.get_surface();
    EXPECT_EQ(sptr->get_type(), FLAT);
    auto fptr = std::dynamic_pointer_cast<Flat>(sptr);
    EXPECT_NE(fptr, nullptr);

    EXPECT_TRUE(ve.is_virtual());
    EXPECT_TRUE(ve.is_single());
    EXPECT_FALSE(ve.is_composite());

    return;
}

TEST(Element, VirtualPlane)
{
    const double LX = 5.0;
    const double LY = 2.5;
    VirtualPlane vp(LX, LY);

    EXPECT_EQ(vp.get_aperture()->get_type(), RECTANGULAR);
    EXPECT_EQ(vp.get_surface()->get_type(), FLAT);

    auto rptr = std::dynamic_pointer_cast<Rectangular>(vp.get_aperture());
    EXPECT_NE(rptr, nullptr);
    if(rptr != nullptr)
    {
        EXPECT_EQ(rptr->x_length, LX);
        EXPECT_EQ(rptr->y_length, LY);
    }

    return;

}

TEST(Element, CompositeElementAccessors)
{
    auto cmp = make_element<CompositeElement>();

    EXPECT_TRUE(cmp->is_composite());

    // Things that should be empty...
    EXPECT_EQ(cmp->get_aperture(), nullptr);
    EXPECT_EQ(cmp->get_surface(), nullptr);
    EXPECT_EQ(cmp->get_front_optical_properties(), nullptr);
    EXPECT_EQ(cmp->get_back_optical_properties(), nullptr);

    // Add/remove/change elements
    const int NUM_ELEMENTS = 4;
    for (int i = 0; i < NUM_ELEMENTS; ++i)
    {
        auto elem = make_element<SingleElement>();
        cmp->add_element(elem);
    }
    EXPECT_EQ(cmp->get_number_of_elements(), NUM_ELEMENTS);

    cmp->remove_element(0);
    EXPECT_EQ(cmp->get_number_of_elements(), NUM_ELEMENTS - 1);
    EXPECT_EQ(cmp->get_element(0), nullptr);
    auto elem = make_element<SingleElement>();
    auto id = cmp->add_element(elem);
    EXPECT_EQ(cmp->get_element(id).get(), elem.get());

    auto elem2 = make_element<SingleElement>();
    EXPECT_NE(elem.get(), elem2.get());
    cmp->replace_element(id, elem2);
    EXPECT_EQ(cmp->get_element(id).get(), elem2.get());

    // Check that pass through functions are hooked up correctly
    auto iter = cmp->get_iterator();
    while (!cmp->is_at_end(iter))
    {
        ++iter;
    }
    EXPECT_TRUE(cmp->is_at_end(iter));

    auto citer = cmp->get_const_iterator();
    while (!cmp->is_at_end(citer))
    {
        ++citer;
    }
    EXPECT_TRUE(cmp->is_at_end(citer));
}

TEST(Element, CompositeElementOrientationUpdate)
{
    // TODO: Implement this test.
    return;
}
