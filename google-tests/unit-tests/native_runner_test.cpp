#include <gtest/gtest.h>

#include <common.hpp>

#include <error_distributions.hpp>
#include <mtrand.hpp>
#include <native_runner.hpp>
#include <native_runner_types.hpp>
#include <ray_source.hpp>
#include <sun.hpp>
#include <simulation_data.hpp>
#include <single_element.hpp>
#include <stage_element.hpp>
#include <vector3d.hpp>

TEST(RandomNumberGenerator, SingleNumberMersenneTwister)
{
    MTRand myrng(1);
    double random_number = myrng.rand();

    EXPECT_NEAR(random_number, 0.13387664401253274, 1e-7);
}

TEST(NativeRunnerTypes, TSun)
{

    SimulationData my_sim;
    auto sun = make_ray_source<Sun>();
    Vector3d spos(1.0, 2.0, 3.0);
    sun->set_position(spos);
    sun->set_shape(PILLBOX);
    my_sim.add_ray_source(sun);

    NativeRunner runner;
    runner.setup_sun(&my_sim);
    auto sys = runner.get_system();
    EXPECT_TRUE(is_identical(sys->Sun.Origin, sun->get_position()));
    EXPECT_EQ(sys->Sun.ShapeIndex, PILLBOX);
}

TEST(NativeRunnerTypes, TElement)
{
    // TODO: Implement a test here...

    // SimulationData my_sim;
    // // **** Setup Answers **** //
    // // Origin
    // Vector3d Origin1(1.0, 2.0, 3.0);
    // // Corresponding Euler angles in radians
    // const double a1 = 0.0;
    // const double b1 = asin(-1.0 / sqrt(3.0));
    // const double g1 = acos(1.0 / cos(b1) * 1.0 / sqrt(6.0)); // approximately 0.615
    // // Corresponding aim vector (local z-axis in reference coordinates)
    // Vector3d aim1(0.0, -1.0 / sqrt(3.0), sqrt(2.0 / 3.0));
    // vector_add(1.0, Origin1, 1.0, aim1);

    // // Z-Rotation is the last of the Euler angles but in degrees
    // const double zrot1 = g1 * 180.0 / M_PI;

    // // Origin
    // Vector3d Origin2(-3.0, 1.0, -5.0);
    // const double a2 = M_PI / 4.0;
    // const double b2 = M_PI / 6.0;
    // const double g2 = M_PI / 3.0;
    // // Corresponding aim vector (local z-axis in reference coordinates)
    // Vector3d aim2(sqrt(3.0 / 8.0), 0.5, sqrt(3.0 / 8.0));
    // vector_add(1.0, Origin2, 1.0, aim2);

    // // Z-Rotation is the last of the Euler angles but in degrees
    // const double zrot2 = 60.0;

    // // **** Setup Elements **** //
    // auto el = make_element<SingleElement>();
    // el->set_aperture(make_aperture<Circle>(2.0));
    // el->set_surface(make_surface<Flat>());
    // el->set_reference_frame_geometry(Origin1, aim1, zrot1);

    // auto st = make_stage(0);
    // st->set_reference_frame_geometry(Origin2, aim2, zrot2);
    // st->add_element(el);
}

TEST(NativeRunnerTypes, TStage)
{
    // TODO: Implement test
}

TEST(NativeRunner, SmokeTest)
{
    NativeRunner runner;
    SimulationData my_sim;

    SimulationParameters &params = my_sim.get_simulation_parameters();
    params.include_optical_errors = false;
    params.include_sun_shape_errors = false;
    params.number_of_rays = 10;
    params.max_number_of_rays = 100;
    // my_sim.set_number_of_rays(10);
    // my_sim.set_max_rays_traced(100);

    auto sun = make_ray_source<Sun>();
    sun->set_position(0.0, 0.0, 100.0);
    sun->set_shape(DistributionType::GAUSSIAN);
    my_sim.add_ray_source(sun);

    auto my_st = make_stage(0);
    const int NUM_ELEMENTS = 4;
    double x[NUM_ELEMENTS] = {1.0, 0.0, -1.0, 0.0};
    double y[NUM_ELEMENTS] = {0.0, 1.0, 0.0, -1.0};
    OpticalProperties optics(InteractionType::REFLECTION,
                             DistributionType::GAUSSIAN,
                             0.0, 1.0, 0.0, 0.0, 1.0, 1.0);
                            //  0.0, 0.0, 0.0, 0.0, 1.0, 1.0);
    for (int k = 0; k < NUM_ELEMENTS; ++k)
    {
        element_ptr el = make_element<SingleElement>();
        el->set_aperture(make_aperture<Circle>(2.0));
        el->set_surface(make_surface<Flat>());
        el->set_reference_frame_geometry(Vector3d(x[k], y[k], 0.0),
                                         Vector3d(-x[k], -y[k], 1.0),
                                         0.0);
        el->set_front_optical_properties(optics);
        el->set_back_optical_properties(optics);
        my_st->add_element(el);
    }
    EXPECT_EQ(my_st->get_number_of_elements(), NUM_ELEMENTS);
    my_sim.add_stage(my_st);
    EXPECT_EQ(my_sim.get_number_of_elements(), NUM_ELEMENTS);

    RunnerStatus sts;
    sts = runner.initialize();
    EXPECT_EQ(sts, RunnerStatus::SUCCESS);
    sts = runner.setup_simulation(&my_sim);
    EXPECT_EQ(sts, RunnerStatus::SUCCESS);
    sts = runner.run_simulation();
    EXPECT_EQ(sts, RunnerStatus::SUCCESS);

    const TSystem *sys = runner.get_system();
    sys->AllRayData.Print();
}

TEST(NativeRunner, PowerTowerSmokeTest)
{
    SimulationData sd;

    // Sun
    auto sun = make_ray_source<Sun>();
    sun->set_position(0.0, 0.0, 100.0);
    sd.add_ray_source(sun);

    // Absorber -- Flat
    auto absorber = make_element<SingleElement>();
    absorber->set_origin(0.0, 0.0, 10.0);
    absorber->set_aim_vector(0.0, 5.0, 0.0);
    absorber->set_zrot(0.0);
    absorber->compute_coordinate_rotations();
    absorber->set_surface(make_surface<Flat>()); // surface(nullptr)
    absorber->set_aperture(make_aperture<Rectangle>(2.0, 2.0));
    OpticalProperties *foptics = absorber->get_front_optical_properties();
    foptics->my_type = REFLECTION;
    foptics->reflectivity = 0.0;

    // Make stage 1 -- second stage -- these can be added to SimulationData
    // in any order but should be numbered in the desired order
    auto st1 = make_stage(1);
    // Origin is initialized to zero but set it explicitly
    st1->set_origin(0.0, 0.0, 0.0);
    // Set aim vector so stage and global coordinates are identical
    st1->set_aim_vector(0.0, 0.0, 1.0);
    // Set z rotation so stage and global coordinates are identical
    st1->set_zrot(0.0);
    // Compute coordinate rotations
    st1->compute_coordinate_rotations();
    st1->add_element(absorber);
    // Optional -- to help the user identify things

    // Make stage 0 -- this will be the first stage if the runner uses stages
    auto st0 = make_stage(0);
    st0->set_origin(0.0, 0.0, 0.0);
    st0->set_aim_vector(0.0, 0.0, 1.0);

    Vector3d rvec, svec, avec;
    Vector3d aim, pos;

    const int NUM_ELEMENTS = 10;
    for (int k = 0; k < NUM_ELEMENTS; ++k)
    {
        auto el = make_element<SingleElement>();
        foptics = el->get_front_optical_properties();
        foptics->reflectivity = 1.0;

        pos.set_values(5 * sin(k * M_PI * 2.0 / NUM_ELEMENTS),
                       5 * cos(k * M_PI * 2.0 / NUM_ELEMENTS),
                       0.0);
        vector_add(1.0, absorber->get_origin_global(),
                   -1.0, pos,
                   rvec);
        make_unit_vector(rvec);
        svec = sun->get_position();
        make_unit_vector(svec);
        vector_add(0.5, rvec, 0.5, svec, avec);
        vector_add(1.0, pos, 100.0, avec, aim);

        el->set_reference_frame_geometry(pos, aim, 0.0);

        el->set_surface(make_surface<Flat>());
        el->set_aperture(make_aperture<Circle>(2.0));

        st0->add_element(el);
    }
    EXPECT_EQ(st0->get_number_of_elements(), NUM_ELEMENTS);
    sd.add_stage(st0);
    EXPECT_EQ(sd.get_number_of_elements(), NUM_ELEMENTS);
    sd.add_stage(st1);
    EXPECT_EQ(sd.get_number_of_elements(), NUM_ELEMENTS + 1);

    // Set parameters
    SimulationParameters &params = sd.get_simulation_parameters();
    params.number_of_rays = 10000;
    params.max_number_of_rays = params.number_of_rays * 100;
    params.include_optical_errors = false;
    params.include_sun_shape_errors = false;
    params.seed = 12345;

    NativeRunner runner;
    RunnerStatus sts = runner.initialize();
    EXPECT_EQ(sts, RunnerStatus::SUCCESS);
    // Setup runs but is not complete
    sts = runner.setup_simulation(&sd);
    EXPECT_EQ(sts, RunnerStatus::SUCCESS);
    // Run simulation runs but returns RunnerStatus::ERROR
    sts = runner.run_simulation();
    EXPECT_EQ(sts, RunnerStatus::SUCCESS);

    // TODO: Do some post processing tests here

    // const TSystem *sys = runner.get_system();
    // // auto ray_data = sys->AllRayData;
    // sys->AllRayData.Print();
}
