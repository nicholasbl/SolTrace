#include <gtest/gtest.h>

#include <arclength.hpp>
#include <native_runner.hpp>
#include <native_runner_types.hpp>
#include <parabolic_dish.hpp>
#include <simulation_data.hpp>
#include <sun.hpp>

// #include "common.hpp"

TEST(ParabolicDish, ArcLength)
{
    const double TOL = 1e-6;
    const double ARC_LENGTH = 4.105679289785514;

    double x0 = -1.0;
    double x1 = 2.0;
    double cx = 1.0;
    // double xstart = -hw;
    // double arc_length = hw * sqrt(hw * this->cx * hw * this->cx + 1) +
    //                     asinh(hw * this->cx) / this->cx;
    double val = parabolic_arc_length(cx, x0, x1, 0.5*TOL);
    EXPECT_NEAR(val, ARC_LENGTH, TOL);

    double xtest = parabolic_determine_x_coordinate(cx, x0, ARC_LENGTH, 0.5*TOL);
    EXPECT_NEAR(xtest, x1, TOL);
}

TEST(ParabolicDish, Build)
{
    OpticalProperties mirror;
    mirror.set_ideal_reflection();

    OpticalProperties absorber;
    absorber.set_ideal_absorption();

    auto dish = make_element<ParabolicDish>();
    dish->set_optics(mirror, absorber);
    dish->set_origin(20.0, -20.0, 30.0);
    dish->set_aperture_size(10.0);
    dish->set_number_of_panels(2, 2);
    dish->set_gaps(0.02, 0.01, 0.5);
    dish->set_focal_length(7.5);
    dish->set_receiver_dimensions(0.25, 7.25);
    dish->create_geometry();

    dish = make_element<ParabolicDish>();
    dish->set_optics(mirror, absorber);
    dish->set_origin(20.0, -20.0, 30.0);
    dish->set_aperture_size(10.0);
    dish->set_number_of_panels(1, 1);
    dish->set_gaps(0.02, 0.01, 0.5);
    dish->set_focal_length(7.5);
    dish->set_receiver_dimensions(0.25, 7.25);
    dish->create_geometry();

    // TODO: Check that everything ends up in the proper position
}

TEST(ParabolicDish, Tracing)
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

    auto dish = make_element<ParabolicDish>();
    dish->set_optics(mirror, absorber);
    dish->set_origin(2.0, -2.0, 0.0);
    dish->set_aperture_size(10.0);
    dish->set_number_of_panels(2, 2);
    dish->set_gaps(0.02, 0.01, 0.5);
    dish->set_focal_length(7.5);
    dish->set_receiver_dimensions(0.5, 7.25);
    dish->set_name("ParabolicDish");
    dish->create_geometry();

    auto sun = make_ray_source<Sun>();
    sun->set_position(0.0, 0.0, 1000.0);
    sun->set_shape(DistributionType::PILLBOX);
    my_sim.add_ray_source(sun);
    
    // Assumes that reference and global coordinates are the same
    dish->set_aim_vector(sun->get_position());
    dish->set_zrot(0.0);
    dish->compute_coordinate_rotations();
    my_sim.add_element(dish);

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
