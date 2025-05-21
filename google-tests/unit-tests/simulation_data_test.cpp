#include <gtest/gtest.h>

#include <composite_element.hpp>
#include <single_element.hpp>
#include <sun.hpp>
#include <simulation_data.hpp>

TEST(SimulationData, AddRemoveGetElements)
{
    // auto my_reflector = std::make_shared<PlaneReflector>();
    SimulationData my_sim;

    auto my_reflector = make_element<SingleElement>();
    auto id1 = my_sim.add_element(my_reflector);
    EXPECT_EQ(id1, my_reflector->get_id());
    EXPECT_EQ(my_sim.get_number_of_elements(), 1);

    auto my_comp = make_element<CompositeElement>();
    auto sub1 = make_element<SingleElement>();
    auto sub2 = make_element<SingleElement>();
    my_comp->add_element(sub1);
    my_comp->add_element(sub2);
    EXPECT_EQ(my_comp->get_number_of_elements(), 2);

    auto id2 = my_sim.add_element(my_comp);
    EXPECT_EQ(id2, my_comp->get_id());
    EXPECT_NE(id1, id2);
    EXPECT_NE(sub1->get_id(), ELEMENT_ID_UNASSIGNED);

    EXPECT_EQ(my_sim.get_number_of_elements(), 3);
    EXPECT_EQ(my_sim.get_element(id1), my_reflector);
    EXPECT_EQ(my_sim.get_element(id2), my_comp);
    EXPECT_EQ(my_sim.get_element(sub1->get_id()), sub1);

    EXPECT_EQ(my_sim.add_element(sub1), ELEMENT_ALREADY_REGISTERED);
    EXPECT_EQ(my_sim.get_number_of_elements(), 3);

    element_id nonexistant = id2 + 1000;
    EXPECT_EQ(my_sim.get_element(nonexistant), nullptr);
    EXPECT_EQ(my_sim.remove_element(nonexistant), 0);
    EXPECT_EQ(my_sim.get_number_of_elements(), 3);

    EXPECT_EQ(my_sim.remove_element(id1), 1);
    EXPECT_EQ(my_sim.get_number_of_elements(), 2);
    EXPECT_EQ(my_sim.get_element(id1), nullptr);

    auto id3 = sub1->get_id();
    EXPECT_EQ(my_sim.remove_element(id2), 2);
    EXPECT_EQ(my_sim.get_number_of_elements(), 0);
    EXPECT_EQ(my_sim.get_element(id2), nullptr);
    EXPECT_EQ(my_sim.get_element(id3), nullptr);

    EXPECT_EQ(my_reflector->get_id(), ELEMENT_ID_UNASSIGNED);
    EXPECT_EQ(my_comp->get_id(), ELEMENT_ID_UNASSIGNED);
    EXPECT_EQ(sub1->get_id(), ELEMENT_ID_UNASSIGNED);
}

TEST(SimulationData, ReplaceElement)
{
    auto el1 = make_element<SingleElement>();
    auto el2 = make_element<SingleElement>();
    auto cmp1 = make_element<CompositeElement>();
    auto sub1 = make_element<SingleElement>();
    auto sub2 = make_element<SingleElement>();
    cmp1->add_element(sub1);
    cmp1->add_element(sub2);
    auto cmp2 = make_element<CompositeElement>();
    auto sub3 = make_element<SingleElement>();
    auto sub4 = make_element<SingleElement>();
    auto sub5 = make_element<SingleElement>();
    cmp2->add_element(sub3);
    cmp2->add_element(sub4);
    cmp2->add_element(sub5);

    SimulationData my_sim;
    auto id1 = my_sim.add_element(el1);
    auto id2 = my_sim.add_element(cmp1);
    EXPECT_EQ(my_sim.get_number_of_elements(), 3);

    // Replace element that is not in the simulation data
    EXPECT_FALSE(my_sim.replace_element(ELEMENT_ID_UNASSIGNED, el2));

    // Replace single element with single element
    EXPECT_TRUE(my_sim.replace_element(id1, el2));
    EXPECT_EQ(el1->get_id(), ELEMENT_ID_UNASSIGNED);
    EXPECT_EQ(el2->get_id(), id1);
    EXPECT_EQ(my_sim.get_element(id1), el2);
    EXPECT_EQ(my_sim.get_number_of_elements(), 3);

    // Replace composite element with single element
    EXPECT_TRUE(my_sim.replace_element(id2, el1));
    EXPECT_EQ(my_sim.get_element(id2), el1);
    EXPECT_EQ(my_sim.get_number_of_elements(), 2);
    EXPECT_EQ(cmp1->get_id(), ELEMENT_ID_UNASSIGNED);
    EXPECT_EQ(sub1->get_id(), ELEMENT_ID_UNASSIGNED);
    EXPECT_EQ(el1->get_id(), id2);

    // Replace single element with composite element
    EXPECT_TRUE(my_sim.replace_element(id2, cmp2));
    EXPECT_EQ(my_sim.get_element(id2), cmp2);
    EXPECT_EQ(my_sim.get_number_of_elements(), 4);
    EXPECT_EQ(el1->get_id(), ELEMENT_ID_UNASSIGNED);
    EXPECT_EQ(cmp2->get_id(), id2);
    EXPECT_NE(sub3->get_id(), ELEMENT_ID_UNASSIGNED);
    EXPECT_EQ(my_sim.get_element(sub3->get_id()), sub3);

    // Replace composite element with composite element
    EXPECT_TRUE(my_sim.replace_element(id2, cmp1));
    EXPECT_EQ(my_sim.get_element(id2), cmp1);
    EXPECT_EQ(my_sim.get_number_of_elements(), 3);
    EXPECT_EQ(cmp2->get_id(), ELEMENT_ID_UNASSIGNED);
    EXPECT_EQ(cmp1->get_id(), id2);
    EXPECT_EQ(sub3->get_id(), ELEMENT_ID_UNASSIGNED);
    EXPECT_NE(sub1->get_id(), ELEMENT_ID_UNASSIGNED);
    EXPECT_EQ(my_sim.get_element(sub1->get_id()), sub1);
}

TEST(SimulationData, IteratorElement)
{
    auto el1 = make_element<SingleElement>();
    auto el2 = make_element<SingleElement>();
    auto cmp1 = make_element<CompositeElement>();
    auto sub1 = make_element<SingleElement>();
    auto sub2 = make_element<SingleElement>();
    cmp1->add_element(sub1);
    cmp1->add_element(sub2);
    auto cmp2 = make_element<CompositeElement>();
    auto sub3 = make_element<SingleElement>();
    auto sub4 = make_element<SingleElement>();
    auto sub5 = make_element<SingleElement>();
    cmp2->add_element(sub3);
    cmp2->add_element(sub4);
    cmp2->add_element(sub5);

    SimulationData my_sim;
    my_sim.add_element(el1);
    my_sim.add_element(el2);
    my_sim.add_element(cmp1);
    my_sim.add_element(cmp2);
    EXPECT_EQ(my_sim.get_number_of_elements(), 7);

    std::set<element_id> ids;
    std::set<element_id> cids;
    for (auto iter = my_sim.get_iterator();
         !my_sim.is_at_end(iter);
         ++iter)
    {
        if (iter->second->is_single())
        {
            ids.insert(iter->first);
        }
        if (iter->second->is_composite())
        {
            cids.insert(iter->first);
        }
        EXPECT_EQ(iter->first, iter->second->get_id());
    }
    EXPECT_EQ(ids.size(), my_sim.get_number_of_elements());
    EXPECT_EQ(cids.size(), 2);

    ids.clear();
    cids.clear();
    for (auto iter = my_sim.get_const_iterator();
         !my_sim.is_at_end(iter);
         ++iter)
    {
        if (iter->second->is_single())
        {
            ids.insert(iter->first);
        }
        if (iter->second->is_composite())
        {
            cids.insert(iter->first);
        }
        EXPECT_EQ(iter->first, iter->second->get_id());
    }
    EXPECT_EQ(ids.size(), my_sim.get_number_of_elements());
    EXPECT_EQ(cids.size(), 2);
}

TEST(SimulationData, RaySourceInterface)
{
    SimulationData my_sim;

    auto src1 = make_ray_source<Sun>();
    auto id1 = my_sim.add_ray_source(src1);
    auto src2 = make_ray_source<Sun>();
    auto id2 = my_sim.add_ray_source(src2);
    auto src3 = make_ray_source<Sun>();

    EXPECT_EQ(my_sim.get_ray_source(id1), src1);
    EXPECT_EQ(my_sim.get_ray_source(id2), src2);
    EXPECT_NE(id1, id2);

    ray_source_id nonexistant = id2 + 1000;
    EXPECT_EQ(my_sim.get_ray_source(nonexistant), nullptr);
    EXPECT_EQ(my_sim.remove_ray_source(nonexistant), 0);
    EXPECT_FALSE(my_sim.replace_ray_source(nonexistant, src3));

    EXPECT_TRUE(my_sim.replace_ray_source(id1, src3));
    EXPECT_EQ(my_sim.get_ray_source(id1), src3);
    EXPECT_EQ(my_sim.remove_ray_source(id1), 1);
}

TEST(SimulationData, ParametersInterface)
{
    SimulationData my_sim;

    // auto params = my_sim.get_simulation_parameters();
    const SimulationParameters &params = my_sim.get_simulation_parameters();
    SimulationParameters &mutable_params = my_sim.get_simulation_parameters();

    const int NRAYS = 10;
    my_sim.set_number_of_rays(NRAYS);
    EXPECT_EQ(my_sim.get_number_of_rays(), NRAYS);

    const double TOL = 1e-8;
    my_sim.set_tolerance(TOL);
    EXPECT_EQ(my_sim.get_tolerance(), TOL);

    const int SEED = 12345;
    my_sim.set_seed(SEED);
    EXPECT_EQ(my_sim.get_seed(), SEED);

    const double LAT = 1.25;
    my_sim.set_latitude(LAT);
    EXPECT_EQ(my_sim.get_latitude(), LAT);

    const double LONG = 0.75;
    my_sim.set_longitude(LONG);
    EXPECT_EQ(my_sim.get_longitude(), LONG);

    // TODO: Add datetime accessor tests

    EXPECT_EQ(params.seed, SEED);
    EXPECT_EQ(mutable_params.seed, SEED);
    mutable_params.seed = 0;
    EXPECT_EQ(my_sim.get_seed(), 0);
}
