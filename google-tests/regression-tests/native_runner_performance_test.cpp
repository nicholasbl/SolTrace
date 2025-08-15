#include <gtest/gtest.h>

#include <aperture.hpp>
#include <element.hpp>
#include <heliostat.hpp>
#include <native_runner.hpp>
#include <simulation_data.hpp>
#include <simulation_result.hpp>
// #include <parabolic_trough.hpp>
// #include <single_element.hpp>
#include <stage_element.hpp>
#include <sun.hpp>
#include <surface.hpp>

TEST(NativeRunner, PerformanceTest)
{
    // TODO: Add timing component to test

    const uint_fast64_t NRAYS = 100000;
    const Vector3d zero(0.0, 0.0, 0.0);
    const Vector3d khat(0.0, 0.0, 1.0);

    const uint_fast64_t NX = 4;
    const uint_fast64_t NY = 2;
    const uint_fast64_t NHX = 20;
    const uint_fast64_t NHY = 12;

    const double LX = 10.0;
    const double LY = 5.0;
    const double dx = 2.0 * LX / NHX;
    const double dy = 2.0 * LY / NHY;
    const double ABS_RADIUS = 10.0;

    std::cout << "Num Elements: " << NX * NY * NHX * NHY << std::endl;
    std::cout << "dx = " << dx << "\ndy = " << dy << std::endl;

    SimulationData sdata;
    NativeRunner my_runner;

    SimulationParameters &params = sdata.get_simulation_parameters();
    params.include_optical_errors = false;
    params.include_sun_shape_errors = false;
    params.max_number_of_rays = NRAYS * 100;
    params.number_of_rays = NRAYS;
    params.seed = 12345;

    my_runner.enable_power_tower();
    my_runner.enable_point_focus();

    OpticalProperties mirror;
    mirror.set_ideal_reflection();

    stage_ptr st1 = make_stage(1);
    st1->set_reference_frame_geometry(zero, khat, 0.0);
    stage_ptr st2 = make_stage(2);
    st2->set_reference_frame_geometry(zero, khat, 0.0);

    Vector3d sun_pos(0.0, 0.0, 10000.0);
    Vector3d abs_origin(0.0, 0.0, 10.0);
    Vector3d hs_origin;
    Vector3d v1;
    Vector3d v2;
    Vector3d aim;
    Vector3d aim_point;

    // double xpos = -1.0 * LX;
    // double ypos = -1.0 * LY;
    double xpos, ypos;

    for (auto ix = 0; ix < NHX; ++ix)
    {
        xpos = dx * ix - LX;
        for (auto jy = 0; jy < NHY; ++jy)
        {
            ypos = dy * jy - LY;
            hs_origin.set_values(xpos, ypos, 0.0);

            vector_add(1.0, sun_pos, -1.0, hs_origin, v1);
            vector_add(1.0, abs_origin, -1.0, hs_origin, v2);
            vector_add(0.5, v1, 0.5, v2, aim);
            // std::cout << aim << std::endl;
            vector_add(1.0, hs_origin, 1.0, aim, aim_point);

            auto hs = make_element<Heliostat>();
            hs->set_optics(mirror);
            hs->set_reference_frame_geometry(hs_origin, aim, 0.0);
            hs->set_aperture_size(2.0 * dx, 2.0 * dy);
            hs->set_number_panels(NX, NY);
            hs->set_gaps(0.0, 0.0);
            hs->set_focal_length(0.0);
            hs->set_canting(Heliostat::NONE, 0.0, 0.0);
            hs->create_geometry();
            hs->set_name("Heliostat");
            hs->enable();

            // std::cout << "****************"
            //           << "\nix = " << ix << "  jy = " << jy
            //           << "\norigin: " << hs_origin
            //           << "\naim: " << aim
            //           << "\naim_point: " << aim_point
            //           << "\nv1: " << v1
            //           << "\nv2: " << v2
            //           << std::endl;

            auto ret = st1->add_element(hs);
            EXPECT_TRUE(Element::is_success(ret));
        }
    }

    auto absorb = make_element<SingleElement>();
    absorb->get_front_optical_properties()->set_ideal_absorption();
    absorb->get_back_optical_properties()->set_ideal_absorption();
    absorb->set_aperture(make_aperture<Circle>(2.0 * ABS_RADIUS));
    absorb->set_surface(make_surface<Sphere>(1.0 / ABS_RADIUS));
    // absorb->set_surface(make_surface<Flat>());

    aim_point = abs_origin;
    aim_point[2] += vector_norm(aim_point);
    absorb->set_reference_frame_geometry(abs_origin, aim_point, 0.0);
    absorb->set_name("Absorber");
    absorb->enable();
    auto ret = st2->add_element(absorb);
    EXPECT_TRUE(Element::is_success(ret));

    sdata.add_stage(st1);
    sdata.add_stage(st2);

    auto sun = make_ray_source<Sun>();
    sun->set_position(sun_pos);
    sun->set_shape(DistributionType::PILLBOX, 0.0, 0.5);
    sdata.add_ray_source(sun);

    RunnerStatus sts = my_runner.initialize();
    EXPECT_EQ(sts, RunnerStatus::SUCCESS);
    // Setup runs but is not complete
    sts = my_runner.setup_simulation(&sdata);
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

    EXPECT_TRUE(n >= NRAYS);
    EXPECT_TRUE(num_absorbed > 0);

    // sys->AllRayData.Print();
}
