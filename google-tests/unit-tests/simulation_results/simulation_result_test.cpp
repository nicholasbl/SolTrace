#include <gtest/gtest.h>

#include <sstream>

#include <simulation_result.hpp>
#include <vector3d.hpp>

#include "common.hpp"

using SolTrace::Result::InteractionRecord;
using SolTrace::Result::RayEvent;
using SolTrace::Result::RayRecord;
using SolTrace::Result::SimulationResult;

using SolTrace::Result::interaction_ptr;
using SolTrace::Result::ray_record_ptr;

using SolTrace::Result::make_interaction_record;
using SolTrace::Result::make_ray_record;

TEST(InteractionRecord, Constructors)
{
    element_id elid = 5;
    RayEvent it = RayEvent::ABSORB;
    Vector3d loc(-3.2, -2.5, 1.2);
    InteractionRecord ir1(elid, it, loc);
    InteractionRecord ir2(elid, it, loc);

    EXPECT_TRUE(ir1.index < 0);
    EXPECT_TRUE(ir2.index < 0);
    EXPECT_EQ(ir1.element, ir2.element);
    EXPECT_EQ(ir1.event, ir2.event);
    EXPECT_EQ(ir1.location[0], ir2.location[0]);
    EXPECT_EQ(ir1.location[1], ir2.location[1]);
    EXPECT_EQ(ir1.location[2], ir2.location[2]);
}

TEST(InteractionRecord, OutputOperator)
{
    element_id elid = 5;
    RayEvent it = RayEvent::ABSORB;
    Vector3d loc(-3.2, -2.5, 1.2);
    InteractionRecord ir1(elid, it, loc);

    // Basic test to see that the output operator is there
    // and functioning.
    std::stringstream ss;
    ss << ir1;
    EXPECT_TRUE(ss.str().size() > 0);
}

TEST(RayRecord, Accessors)
{
    // Test constants
    const double TOL = 1e-12;
    const uint_fast32_t NINTER = 5;
    const int_fast64_t ID = 5;
    RayEvent my_types[NINTER] = {
        RayEvent::CREATE,
        RayEvent::TRANSMIT,
        RayEvent::REFLECT,
        RayEvent::ABSORB,
        RayEvent::EXIT
    };

    // Adding records and sizing
    ray_record_ptr rr = make_ray_record(ID);
    for (uint_fast32_t ell = 0; ell < NINTER; ++ell)
    {
        Vector3d loc(1.0 * ell * ell, 2.0 * ell * ell, 3.0 * ell * ell);
        RayEvent it = my_types[ell];
        interaction_ptr ir = make_interaction_record(ell, it, loc);
        rr->add_interaction_record(ir);
        EXPECT_EQ(rr->get_number_of_interactions(), ell + 1);
    }

    // Setting last direction cosines
    Vector3d lcos(1.0, 2.0, 3.0);
    rr->set_last_cosines(lcos[0], lcos[1], lcos[2]);
    EXPECT_TRUE(is_identical(rr->cos_last, lcos));
    lcos.set_values(3.0, 4.0, 5.0);
    rr->set_last_cosines(lcos);
    EXPECT_TRUE(is_identical(rr->cos_last, lcos));

    // Getting direction cosines and indexing
    int_fast64_t idx = 0;
    Vector3d ret1;
    Vector3d ret2;
    for (auto iter = rr->interactions.cbegin();
         iter != rr->interactions.cend();
         ++iter)
    {
        EXPECT_EQ(*iter, (*rr)[idx]);
        rr->get_direction_cosines(idx, ret1);
        rr->get_direction_cosines(*iter, ret2);
        EXPECT_TRUE(is_identical(ret1, ret2));
        if (idx + 1 == NINTER)
        {
            lcos.set_values(3.0, 4.0, 5.0);
        }
        else
        {
            lcos.set_values(1.0, 2.0, 3.0);
            lcos.scalar_mult(2*idx + 1);
        }
        make_unit_vector(lcos);
        EXPECT_NEAR(ret1[0], lcos[0], TOL);
        EXPECT_NEAR(ret1[1], lcos[1], TOL);
        EXPECT_NEAR(ret1[2], lcos[2], TOL);
        ++idx;
    }
}

TEST(RayRecord, OutputOperator)
{
    // Test constants
    const uint_fast32_t NINTER = 5;
    const int_fast64_t ID = 5;
    RayEvent my_types[NINTER] = {
        RayEvent::CREATE,
        RayEvent::TRANSMIT,
        RayEvent::REFLECT,
        RayEvent::ABSORB,
        RayEvent::EXIT
    };

    // Adding records and sizing
    RayRecord rr(ID);
    for (uint_fast32_t ell = 0; ell < NINTER; ++ell)
    {
        Vector3d loc(1.0 * ell * ell, 2.0 * ell * ell, 3.0 * ell * ell);
        RayEvent it = my_types[ell];
        interaction_ptr ir = make_interaction_record(ell, it, loc);
        rr.add_interaction_record(ir);
    }

    std::stringstream ss;
    ss << rr;
    EXPECT_TRUE(ss.str().size() > 0);
}

TEST(SimulationResult, Accessors)
{
    // Test constants
    const uint_fast32_t NRAYS = 3;
    const uint_fast32_t NINTER = 5;
    RayEvent my_types[NINTER] = {
        RayEvent::REFLECT,
        RayEvent::TRANSMIT,
        RayEvent::REFLECT,
        RayEvent::ABSORB,
        RayEvent::EXIT
    };

    SimulationResult sr;

    for (uint_fast32_t k = 0; k < NRAYS; ++k)
    {
        ray_record_ptr rr = make_ray_record(k);
        for (uint_fast32_t ell = 0; ell < NINTER; ++ell)
        {
            Vector3d loc(1.0 * ell, 2.0 * ell, 3.0 * ell);
            RayEvent it = my_types[ell];
            interaction_ptr ir = make_interaction_record(ell, it, loc);
            rr->add_interaction_record(ir);
        }
        sr.add_ray_record(rr);
        EXPECT_EQ(sr.get_number_of_records(), k + 1);
        EXPECT_EQ(sr[k], rr);
    }

    int_fast64_t idx = 0;
    for (auto iter = sr.get_ray_record_iteratior(); !sr.is_at_end(iter); ++iter)
    {
        EXPECT_EQ(*iter, sr[idx]);
        ++idx;
    }
    EXPECT_EQ(sr.get_number_of_records(), idx);
}

TEST(SimulationResult, OstreamOperator)
{

    // Test constants
    const uint_fast32_t NRAYS = 3;
    const uint_fast32_t NINTER = 5;
    RayEvent my_types[NINTER] = {
        RayEvent::REFLECT,
        RayEvent::TRANSMIT,
        RayEvent::REFLECT,
        RayEvent::ABSORB,
        RayEvent::EXIT
    };

    SimulationResult sr;

    for (uint_fast32_t k = 0; k < NRAYS; ++k)
    {
        ray_record_ptr rr = make_ray_record(k);
        for (uint_fast32_t ell = 0; ell < NINTER; ++ell)
        {
            Vector3d loc(1.0 * ell, 2.0 * ell, 3.0 * ell);
            RayEvent it = my_types[ell];
            interaction_ptr ir = make_interaction_record(ell, it, loc);
            rr->add_interaction_record(ir);
        }
        sr.add_ray_record(rr);
    }

    std::stringstream ss;
    ss << sr;

    EXPECT_TRUE(ss.str().size() > 0);
}
