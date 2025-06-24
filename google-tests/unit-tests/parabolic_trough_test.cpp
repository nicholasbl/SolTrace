#include <gtest/gtest.h>

#include <native_runner.hpp>
#include <native_runner_types.hpp>
#include <parabolic_trough.hpp>
#include <simulation_data.hpp>
#include <sun.hpp>

// #include "common.hpp"

TEST(ParabolicTrough, Build)
{
    OpticalProperties mirror;
    mirror.set_ideal_reflection();

    OpticalProperties absorber;
    absorber.set_ideal_absorption();

    OpticalProperties envelop_out;
    envelop_out.set_ideal_transmission();
    envelop_out.refraction_index_front = 1.46;
    envelop_out.refraction_index_back = 1.0;

    OpticalProperties envelop_in;
    envelop_in.set_ideal_transmission();
    envelop_in.refraction_index_front = 1.0;
    envelop_in.refraction_index_back = 1.46;

    auto pt = make_element<ParabolicTrough>();
    pt->set_optics(mirror, absorber, envelop_out, envelop_in);
    pt->set_origin(20.0, -20.0, 30.0);
    pt->set_aperture_size(5.774, 11.96);
    pt->set_number_panels(4, 7);
    pt->set_gaps(0.02, 0.01, 0.08);
    pt->set_focal_length(1.71);
    pt->set_receiver_dimensions(0.07, 0.115, 0.003);
    pt->set_angles(0.0, 0.0);
    pt->create_geometry();

    pt = make_element<ParabolicTrough>();
    pt->set_optics(mirror, absorber, envelop_out, envelop_in);
    pt->set_origin(20.0, -20.0, 30.0);
    pt->set_aperture_size(5.774, 11.96);
    pt->set_number_panels(1, 7);
    pt->set_gaps(0.0, 0.01, 0.0);
    pt->set_focal_length(1.71);
    pt->set_receiver_dimensions(0.07, 0.115, 0.003);
    pt->set_angles(0.0, 0.0);
    pt->create_geometry();

    // TODO: Check that everything ends up in the proper position
}

TEST(ParabolicTrough, Tracing)
{
    const uint_fast64_t NRAYS = 10000;

    SimulationData my_sim;
    // Set parameters
    SimulationParameters &params = my_sim.get_simulation_parameters();
    params.number_of_rays = NRAYS;
    params.max_number_of_rays = params.number_of_rays * 100;
    params.include_optical_errors = true;
    params.include_sun_shape_errors = true;
    params.seed = 123;
    
    NativeRunner my_runner;
    my_runner.disable_power_tower();
    my_runner.disable_point_focus();

    OpticalProperties mirror;
    mirror.set_ideal_reflection();
    mirror.slope_error = 1.5;
    mirror.specularity_error = 0.5;

    OpticalProperties absorber;
    absorber.set_ideal_absorption();
    absorber.slope_error = 1e-5;
    absorber.specularity_error = 1e-5;

    OpticalProperties envelop_out;
    envelop_out.set_ideal_transmission();
    envelop_out.refraction_index_front = 1.46;
    envelop_out.refraction_index_back = 1.0;
    envelop_out.slope_error = 1e-4;
    envelop_out.specularity_error = 1e-4;

    OpticalProperties envelop_in;
    envelop_in.set_ideal_transmission();
    envelop_in.refraction_index_front = 1.0;
    envelop_in.refraction_index_back = 1.46;
    envelop_in.slope_error = 1e-4;
    envelop_out.specularity_error = 1e-4;

    auto pt = make_element<ParabolicTrough>();
    pt->set_optics(mirror, absorber, envelop_out, envelop_in);
    pt->set_origin(20.0, -20.0, 30.0);
    pt->set_aperture_size(6.0, 12.0);
    pt->set_number_panels(4, 7);
    pt->set_gaps(0.02, 0.01, 0.08);
    pt->set_focal_length(1.71);
    pt->set_receiver_dimensions(0.07, 0.115, 0.003);
    pt->set_angles(0.0, 0.0);
    pt->create_geometry();
    pt->set_name("Parabolic Trough");

    auto sun = make_ray_source<Sun>();
    sun->set_position(0.0, 0.0, 1000.0);
    sun->set_shape(DistributionType::PILLBOX);
    my_sim.add_ray_source(sun);
    
    // Assumes that reference and global coordinates are the same
    // Vector3d pt_aim_point;
    // vector_add(1.0, sun->get_position(), -1.0, pt->get_origin_ref(), pt_aim_point);
    // pt->set_aim_vector(pt_aim_point);
    pt->set_aim_vector(sun->get_position());
    pt->set_zrot(0.0);
    pt->compute_coordinate_rotations();
    my_sim.add_element(pt);

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
