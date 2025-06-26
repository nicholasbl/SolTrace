#include <gtest/gtest.h>

#include <heliostat.hpp>
#include <native_runner.hpp>
#include <native_runner_types.hpp>
#include <simulation_data.hpp>
#include <stage_element.hpp>
#include <sun.hpp>

// #include "common.hpp"

TEST(HeliotStat, BuildParabolaNone)
{
    OpticalProperties mirror;
    mirror.set_ideal_reflection();

    auto hs = make_element<Heliostat>();
    hs->set_optics(mirror);
    hs->set_origin(1.0, 1.0, 0.0);
    hs->set_aim_vector(0.0, 0.0, 100.0);
    hs->set_aperture_size(12.0, 12.0);
    hs->set_number_panels(3, 4);
    hs->set_gaps(0.1, 0.1);
    hs->set_focal_length(156.06);
    hs->set_canting(Heliostat::NONE, 0.0, 0.0);
    hs->create_geometry();

    // TODO: Check that everything ends up in the proper position
}

TEST(HeliotStat, BuildFlatOnAxis)
{
    OpticalProperties mirror;
    mirror.set_ideal_reflection();

    auto hs = make_element<Heliostat>();
    hs->set_optics(mirror);
    hs->set_origin(1.0, 1.0, 0.0);
    hs->set_aim_vector(0.0, 0.0, 100.0);
    hs->set_aperture_size(12.0, 12.0);
    hs->set_number_panels(3, 4);
    hs->set_gaps(0.1, 0.1);
    hs->set_focal_length(0.0);
    hs->set_canting(Heliostat::NONE, 0.0, 0.0);
    hs->create_geometry();

    // TODO: Check that everything ends up in the proper position
}

TEST(HelioStat, Trace)
{
    const uint_fast64_t NRAYS = 10000;
    const Vector3d zero(0.0, 0.0, 0.0);
    const Vector3d khat(0.0, 0.0, 1.0);

    SimulationData my_sim;
    // Set parameters
    SimulationParameters &params = my_sim.get_simulation_parameters();
    params.number_of_rays = NRAYS;
    params.max_number_of_rays = params.number_of_rays * 100;
    params.include_optical_errors = false;
    params.include_sun_shape_errors = false;
    params.seed = 12345;

    NativeRunner my_runner;
    my_runner.disable_power_tower();
    my_runner.enable_point_focus();

    OpticalProperties mirror;
    mirror.set_ideal_reflection();

    stage_ptr st1 = make_stage(1);
    st1->set_reference_frame_geometry(zero, khat, 0.0);
    stage_ptr st2 = make_stage(2);
    st2->set_reference_frame_geometry(zero, khat, 0.0);

    Vector3d sun_pos(0.0, 0.0, 1000.0);
    Vector3d hs_origin(1.0, 1.0, 0.0);
    Vector3d abs_origin(0.0, 0.0, 10.0);
    Vector3d v1;
    Vector3d v2;
    Vector3d aim;
    Vector3d aim_point;
    vector_add(1.0, sun_pos, -1.0, hs_origin, v1);
    vector_add(1.0, abs_origin, -1.0, hs_origin, v2);
    vector_add(0.5, v1, 0.5, v2, aim);
    vector_add(1.0, hs_origin, 1.0, aim, aim_point);

    auto hs = make_element<Heliostat>();
    hs->set_optics(mirror);
    // hs->set_origin(hs_origin);
    // hs->set_aim_vector(0.0, 0.0, 2.0);
    // hs->set_zrot(0.0);
    // hs->compute_coordinate_rotations();
    hs->convert_global_to_local(aim, abs_origin);
    hs->set_reference_frame_geometry(hs_origin, aim, 0.0);
    hs->set_aperture_size(12.0, 12.0);
    hs->set_number_panels(3, 4);
    hs->set_gaps(0.1, 0.1);
    hs->set_focal_length(0.0);
    hs->set_canting(Heliostat::NONE, 0.0, 0.0);
    hs->create_geometry();
    hs->set_name("Heliostat");
    hs->enable();

    auto ret = st1->add_element(hs);
    EXPECT_TRUE(Element::is_success(ret));

    auto absorb = make_element<SingleElement>();
    absorb->get_front_optical_properties()->set_ideal_absorption();
    absorb->get_back_optical_properties()->set_ideal_absorption();
    absorb->set_aperture(make_aperture<Rectangle>(5.0, 5.0));
    absorb->set_surface(make_surface<Flat>());
    // absorb->set_origin(abs_origin);
    // absorb->set_aim_vector(0.0, 0.0, -1.0);
    // absorb->set_zrot(0.0);
    // absorb->compute_coordinate_rotations();
    // aim.scalar_mult(-1.0);
    vector_add(1.0, hs_origin, -1.0, abs_origin, aim);
    vector_add(1.0, abs_origin, 1.0, aim, aim_point);
    absorb->set_reference_frame_geometry(abs_origin, aim_point, 0.0);
    absorb->set_name("Absorber");
    absorb->enable();
    ret = st2->add_element(absorb);
    EXPECT_TRUE(Element::is_success(ret));

    my_sim.add_stage(st1);
    my_sim.add_stage(st2);

    auto sun = make_ray_source<Sun>();
    sun->set_position(sun_pos);
    sun->set_shape(DistributionType::PILLBOX);
    my_sim.add_ray_source(sun);

    // // We can go over all the elements added
    // for (auto iter = my_sim.get_iterator();
    //      !my_sim.is_at_end(iter);
    //      ++iter)
    // {
    //     // iter is a iterator over the storing container which is a map
    //     // so that the iterator gives the key value pair
    //     element_id id = iter->first;
    //     // `element_ptr` is a std::shared_pointer to an Element
    //     element_ptr el = iter->second;
    //     if (el->is_stage())
    //     {
    //         continue;
    //     }
    //     std::cout << "------------\n"
    //               << "Element ID: " << id
    //               << "\nElement name: " << el->get_name()
    //               << "\nIs Stage: " << el->is_stage()
    //               << "\nIs Composite: " << el->is_composite()
    //               << "\nIs Single: " << el->is_single()
    //               // Below are all the same in this case
    //               << "\nOrigin (ref): " << el->get_origin_ref()
    //               << "\nOrigin (stage): " << el->get_origin_stage()
    //               << "\nOrigin (global): " << el->get_origin_global()
    //               << "\nAim (ref): " << el->get_aim_vector_ref()
    //               << "\nAim (stage): " << el->get_aim_vector_stage()
    //               << "\nAim (global): " << el->get_aim_vector_global()
    //               << "\n";
    // }

    // std::cout << "Number of elements in sim data: "
    //           << my_sim.get_number_of_elements()
    //           << std::endl;

    RunnerStatus sts = my_runner.initialize();
    EXPECT_EQ(sts, RunnerStatus::SUCCESS);
    // Setup runs but is not complete
    sts = my_runner.setup_simulation(&my_sim);
    EXPECT_EQ(sts, RunnerStatus::SUCCESS);
    // Run simulation runs but returns RunnerStatus::ERROR
    sts = my_runner.run_simulation();
    EXPECT_EQ(sts, RunnerStatus::SUCCESS);

    const TSystem *sys = my_runner.get_system();
    // sys->AllRayData.Print();
    const TRayData *ray_data = &(sys->AllRayData);
    size_t n = ray_data->Count();
    uint_fast64_t num_absorbed = 0;
    for (size_t i = 0; i < n; i++)
    {
        double pos[3], cos[3];
        int elm, stage;
        unsigned int ray;
        if (ray_data->Query(i, pos, cos, &elm, &stage, &ray))
        {
            if (elm < 0)
                ++num_absorbed;
        }
    }

    std::cout << "Number Absorbed: " << num_absorbed << std::endl;
    std::cout << "Number Interactions: " << n << std::endl;

    // ray_data->Print();

    EXPECT_TRUE(n >= NRAYS);
    EXPECT_TRUE(num_absorbed > 0);
}
