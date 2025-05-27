#include <gtest/gtest.h>

#include <composite_element.hpp>
#include <element.hpp>
#include <single_element.hpp>
#include <virtual_element.hpp>
#include <vector3d.hpp>

#include "common.hpp"

TEST(Element, ConstructionSmokeTest)
{
    // Smoke tests
    auto elem = make_element<SingleElement>();
    auto comp = make_element<CompositeElement>();
    auto ve = make_element<VirtualElement>();
    auto vp = make_element<VirtualPlane>(10.0, 10.0);
    auto st = make_stage(1);
}

TEST(Element, SingleElementAccessors)
{
    SingleElement ref;
    Vector3d zero(0.0, 0.0, 0.0);
    Vector3d khat(0.0, 0.0, 1.0);
    EXPECT_TRUE(is_identical(ref.get_origin_local(), zero));
    EXPECT_TRUE(is_identical(ref.get_aim_vector_local(), khat));
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
    EXPECT_TRUE(is_identical(ref.get_origin_local(), pos));

    auto aim = Vector3d(-1.0, 0.0, 1.0);
    ref.set_aim_vector(aim);
    EXPECT_TRUE(is_identical(ref.get_aim_vector_local(), aim));

    // auto eulers = Vector3d(0.1, 0.2, -0.3);
    // // ref.set_euler_angles(eulers);
    // EXPECT_TRUE(is_identical(ref.get_euler_angles(), eulers));

    const double ZROT = 0.58;
    ref.set_zrot(ZROT);
    EXPECT_EQ(ref.get_zrot(), ZROT);

    EXPECT_EQ(ref.set_reference_frame_geometry(pos, aim, ZROT), 0);
    EXPECT_FALSE(is_identical(ref.get_euler_angles(), zero));

    const double D = 1.0;
    auto ap = make_aperture<Circle>(D);
    ref.set_aperture(ap);
    auto rap = std::dynamic_pointer_cast<Circle>(ref.get_aperture());
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

    OpticalProperties op(REFLECTION, 0.75, 0.25, 0.1, 0.001, 1.0, 1.0);
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
}

TEST(Element, VirtualElement)
{
    VirtualElement ve;

    EXPECT_TRUE(ve.is_virtual());
    
    const double LX = 1.0;
    const double LY = 2.0;
    ve.set_aperture(make_aperture<Rectangle>(LX, LY));
    auto aptr = ve.get_aperture();
    EXPECT_EQ(aptr->get_type(), RECTANGLE);
    auto rptr = std::dynamic_pointer_cast<Rectangle>(aptr);
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

    // These functions should have no effects
    OpticalProperties op(REFLECTION, 0.75, 0.25, 0.1, 0.001, 1.0, 1.0);
    ve.set_front_optical_properties(op);
    auto opf = ve.get_front_optical_properties();
    EXPECT_EQ(opf->reflectivity, 0.0);
    EXPECT_EQ(opf->slope_error, 0.0);
    EXPECT_EQ(opf->specularity_error, 0.0);
    EXPECT_EQ(opf->transmitivity, 1.0);
    ve.set_back_optical_properties(op);
    auto opb = ve.get_back_optical_properties();
    EXPECT_EQ(opb->reflectivity, 0.0);
    EXPECT_EQ(opb->slope_error, 0.0);
    EXPECT_EQ(opb->specularity_error, 0.0);
    EXPECT_EQ(opb->transmitivity, 1.0);

    return;
}

TEST(Element, VirtualPlane)
{
    const double LX = 5.0;
    const double LY = 2.5;
    VirtualPlane vp(LX, LY);

    EXPECT_TRUE(vp.is_virtual());
    EXPECT_EQ(vp.get_aperture()->get_type(), RECTANGLE);
    EXPECT_EQ(vp.get_surface()->get_type(), FLAT);

    auto rptr = std::dynamic_pointer_cast<Rectangle>(vp.get_aperture());
    EXPECT_NE(rptr, nullptr);
    if(rptr != nullptr)
    {
        EXPECT_EQ(rptr->x_length, LX);
        EXPECT_EQ(rptr->y_length, LY);
    }

    // These functions should have no effects
    vp.set_aperture(make_aperture<Circle>());
    EXPECT_EQ(vp.get_aperture()->get_type(), RECTANGLE);
    vp.set_surface(make_surface<Parabolic>());
    EXPECT_EQ(vp.get_surface()->get_type(), FLAT);

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
    const aperture_ptr ap = cmp->get_aperture();
    EXPECT_EQ(ap, nullptr);
    const surface_ptr sp = cmp->get_surface();
    EXPECT_EQ(sp, nullptr);
    const OpticalProperties* op = cmp->get_back_optical_properties();
    EXPECT_EQ(op, nullptr);
    op = cmp->get_front_optical_properties();
    EXPECT_EQ(op, nullptr);

    // These should do nothing...
    cmp->set_front_optical_properties(OpticalProperties());
    cmp->set_back_optical_properties(OpticalProperties());

    // Add/remove/change elements
    const int NUM_ELEMENTS = 4;
    for (int i = 0; i < NUM_ELEMENTS; ++i)
    {
        auto elem = make_element<SingleElement>();
        cmp->add_element(elem);
        // EXPECT_EQ(elem->get_stage(), STAGE);
    }
    EXPECT_EQ(cmp->get_number_of_elements(), NUM_ELEMENTS);
    // EXPECT_EQ(cmp->get_stage(), cmp->get_element(0)->get_stage());

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

    // Recursive CompositeElements -- this is disallowed!
    auto cmp2 = make_element<CompositeElement>();
    auto cid = cmp->add_element(cmp2);
    EXPECT_FALSE(Element::is_success(cid));
    EXPECT_EQ(cid, ELEMENT_INVALID_SETUP);
    EXPECT_EQ(cmp->get_number_of_elements(), NUM_ELEMENTS);

    elem2->enable();
    cmp->disable();
    EXPECT_FALSE(cmp->is_enabled());
    EXPECT_FALSE(elem2->is_enabled());
    cmp->enable();
    EXPECT_TRUE(cmp->is_enabled());
    EXPECT_TRUE(elem2->is_enabled());

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
}

TEST(Element, StageElementAccessors)
{
    const int_fast64_t STAGE = 10;
    const int_fast64_t RESET_STAGE = 20;
    auto st1 = make_stage(10);

    auto el1 = make_element<SingleElement>();
    auto cmp1 = make_element<CompositeElement>();
    auto sub1 = make_element<SingleElement>();
    auto sub2 = make_element<VirtualElement>();
    auto sub3 = make_element<SingleElement>();
    EXPECT_TRUE(Element::is_success(cmp1->add_element(sub1)));
    EXPECT_TRUE(Element::is_success(cmp1->add_element(sub2)));
    EXPECT_TRUE(Element::is_success(cmp1->add_element(sub3)));

    EXPECT_TRUE(Element::is_success(st1->add_element(el1)));
    EXPECT_TRUE(Element::is_success(st1->add_element(cmp1)));

    EXPECT_EQ(st1->get_number_of_elements(), 4);
    EXPECT_EQ(st1->get_stage(), STAGE);
    EXPECT_EQ(el1->get_stage(), STAGE);
    EXPECT_EQ(cmp1->get_stage(), STAGE);
    EXPECT_EQ(sub1->get_stage(), STAGE);

    st1->set_stage(RESET_STAGE);
    EXPECT_EQ(st1->get_stage(), RESET_STAGE);
    EXPECT_EQ(el1->get_stage(), RESET_STAGE);
    EXPECT_EQ(cmp1->get_stage(), RESET_STAGE);
    EXPECT_EQ(sub1->get_stage(), RESET_STAGE);
}

TEST(Element, CoordinateComputationsSmokeTests)
{
    // TODO: Make more robust test. This is better than nothing for now.
    auto el = make_element<SingleElement>();
    auto st = make_stage(0);
    Vector3d origin(0.0, 0.0, 0.0);
    Vector3d aim(0.0, 0.0, 1.0);
    Vector3d local(1.0, 1.0, 1.0);
    Vector3d ref(1.0, 1.0, 1.0);
    Vector3d result;
    result.zero();
    double zrot = 0.0;

    el->set_reference_frame_geometry(origin, aim, zrot);
    st->set_reference_frame_geometry(origin, aim, zrot);

    // Conversion test
    el->convert_local_to_global(result, local);
    EXPECT_TRUE(is_identical(ref, result));
    result.zero();
    el->convert_local_to_stage(result, local);
    EXPECT_TRUE(is_identical(ref, result));
    result.zero();
    el->convert_local_to_reference(result, local);
    EXPECT_TRUE(is_identical(ref, result));
    result.zero();

    st->convert_local_to_global(result, local);
    EXPECT_TRUE(is_identical(ref, result));
    result.zero();
    st->convert_local_to_stage(result, local);
    EXPECT_TRUE(is_identical(ref, result));
    result.zero();
    st->convert_local_to_reference(result, local);
    EXPECT_TRUE(is_identical(ref, result));
    result.zero();

    st->add_element(el);
    el->convert_local_to_global(result, local);
    EXPECT_TRUE(is_identical(ref, result));
    result.zero();
    el->convert_local_to_stage(result, local);
    EXPECT_TRUE(is_identical(ref, result));
    result.zero();
    el->convert_local_to_reference(result, local);
    EXPECT_TRUE(is_identical(ref, result));
    result.zero();

    // Origin location tests
    EXPECT_TRUE(is_identical(origin, st->get_origin_local()));
    EXPECT_TRUE(is_identical(origin, st->get_origin_stage()));
    EXPECT_TRUE(is_identical(origin, st->get_origin_global()));

    EXPECT_TRUE(is_identical(origin, el->get_origin_local()));
    EXPECT_TRUE(is_identical(origin, el->get_origin_stage()));
    EXPECT_TRUE(is_identical(origin, el->get_origin_global()));

    // Aim vector conversion tests
    EXPECT_TRUE(is_identical(aim, st->get_aim_vector_local()));
    EXPECT_TRUE(is_identical(aim, st->get_aim_vector_stage()));
    EXPECT_TRUE(is_identical(aim, st->get_aim_vector_global()));

    EXPECT_TRUE(is_identical(aim, el->get_aim_vector_local()));
    EXPECT_TRUE(is_identical(aim, el->get_aim_vector_stage()));
    EXPECT_TRUE(is_identical(aim, el->get_aim_vector_global()));

    // Operators
    // TODO: Add some tests here...
    Matrix3d RtoL = el->get_reference_to_local();
    Matrix3d LtoR = el->get_local_to_reference();
    Matrix3d StoL = el->get_stage_to_local();
    Matrix3d LtoS = el->get_local_to_stage();
    Matrix3d GtoL = el->get_global_to_local();
    Matrix3d LtoG = el->get_local_to_global();
}
