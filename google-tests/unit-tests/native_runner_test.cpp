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
#include <filesystem>
#include <virtual_element.hpp>

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
    double NaN = std::numeric_limits<double>::quiet_NaN();
    sun->set_shape(PILLBOX, NaN, NaN);
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
    double NaN = std::numeric_limits<double>::quiet_NaN();
    sun->set_shape(DistributionType::GAUSSIAN, NaN, NaN);
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
    const uint_fast64_t NRAYS = 10000;
    SimulationParameters &params = sd.get_simulation_parameters();
    params.number_of_rays = NRAYS;
    params.max_number_of_rays = params.number_of_rays * 100;
    params.include_optical_errors = false;
    params.include_sun_shape_errors = false;
    params.seed = 12345;

    NativeRunner runner;
    RunnerStatus sts = runner.initialize();
    EXPECT_EQ(sts, RunnerStatus::SUCCESS);
    sts = runner.setup_simulation(&sd);
    EXPECT_EQ(sts, RunnerStatus::SUCCESS);
    sts = runner.run_simulation();
    EXPECT_EQ(sts, RunnerStatus::SUCCESS);

    // TODO: Do some post processing tests here

    // const TSystem *sys = runner.get_system();
    // // auto ray_data = sys->AllRayData;
    // sys->AllRayData.Print();

    const TSystem *sys = runner.get_system();
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

TEST(NativeRunner, SingleRayValidationTest)
{
    const double TOL = 5e-5;
    SimulationData sd;
    // NativeRunner runner;

    // Set parameters
    const uint_fast64_t NRAYS = 1;
    SimulationParameters &params = sd.get_simulation_parameters();
    params.number_of_rays = NRAYS;
    params.max_number_of_rays = params.number_of_rays * 100;
    params.include_optical_errors = false;
    params.include_sun_shape_errors = false;
    params.seed = 1;

    // Sun
    auto sun = make_ray_source<Sun>();
    sun->set_position(0.0, 0.0, 100.0);
    sun->set_shape(DistributionType::PILLBOX);
    sd.add_ray_source(sun);

    auto sph = make_element<SingleElement>();
    Vector3d origin(0.0, 0.0, 15.0);
    Vector3d aim(0.0, 0.0, -1.0);
    double zrot = 0.0;
    sph->set_reference_frame_geometry(origin, aim, zrot);
    sph->set_aperture(make_aperture<Hexagon>(20.0));
    sph->set_surface(make_surface<Sphere>(0.09));
    sph->get_front_optical_properties()->set_ideal_reflection();
    sph->get_back_optical_properties()->set_ideal_reflection();
    sd.add_element(sph);

    auto para = make_element<VirtualElement>();
    origin.set_values(0.0, 0.0, -1.0);
    aim.set_values(0.0, 0.0, 0.0);
    zrot = 0.0;
    para->set_reference_frame_geometry(origin, aim, zrot);
    para->set_aperture(make_aperture<Rectangle>(31.0, 31.0));
    para->set_surface(make_surface<Parabola>(0.5 / 0.03, 0.5 / 0.03));
    sd.add_element(para);

    NativeRunner runner;
    RunnerStatus sts = runner.initialize();
    EXPECT_EQ(sts, RunnerStatus::SUCCESS);
    sts = runner.setup_simulation(&sd);
    EXPECT_EQ(sts, RunnerStatus::SUCCESS);
    sts = runner.run_simulation();
    EXPECT_EQ(sts, RunnerStatus::SUCCESS);

    const TSystem *sys = runner.get_system();
    // sys->AllRayData.Print();
    const TRayData *ray_data = &(sys->AllRayData);
    size_t n = ray_data->Count();

    sys->AllRayData.Print();

    EXPECT_EQ(n, 1);

    Vector3d ipoint, idir;
    int element, stage;
    unsigned int raynum;
    sys->AllRayData.Query(0, ipoint.data, idir.data,
                          &element, &stage, &raynum);

    EXPECT_NEAR(ipoint[0], -3.06214, TOL);
	EXPECT_NEAR(ipoint[1], 5.92862, TOL);
	EXPECT_NEAR(ipoint[2], 12.7732, TOL);

	EXPECT_NEAR(idir[0], 0.0, TOL);
	EXPECT_NEAR(idir[1], 0.0, TOL);
	EXPECT_NEAR(idir[2], -1.0, TOL);
}

TEST(NativeRunner, LegacyFileLoadTest)
{
    std::string project_path = std::string(PROJECT_DIR);
    std::string parent_dir = std::filesystem::path(PROJECT_DIR).parent_path().string();
    std::string sample_path = parent_dir + std::string("/simple_test_case.stinput");

	// Load simulation data from file
    SimulationData sd;
    sd.import_from_file(sample_path);

    // Set parameters
    SimulationParameters& params = sd.get_simulation_parameters();
    params.number_of_rays = 10000;
    params.max_number_of_rays = params.number_of_rays * 100;
    params.include_optical_errors = false;
    params.include_sun_shape_errors = false;
    params.seed = 12345;

	// Create and run the native runner
    NativeRunner runner;
	RunnerStatus sts = runner.initialize();
    EXPECT_EQ(sts, RunnerStatus::SUCCESS);
    sts = runner.setup_simulation(&sd);
    EXPECT_EQ(sts, RunnerStatus::SUCCESS);
    sts = runner.run_simulation();
    EXPECT_EQ(sts, RunnerStatus::SUCCESS);
}