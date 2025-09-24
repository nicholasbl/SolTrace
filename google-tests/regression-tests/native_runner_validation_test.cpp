#include <gtest/gtest.h>

#include <native_runner.hpp>
#include <simulation_data.hpp>
#include <simulation_data_export.hpp>
#include <vector3d.hpp>

#include "split_csv.h"

using namespace std;

TEST(NativeRunner, ValidationTest)
{
    // Pulling in path variable from CMake and creating path to .stinput sample file
    string sample_path = string(PROJECT_DIR) + string("/High Flux Solar Furnace.stinput");

    // Path to .csv exported from Soltrace as ground truth
    string ground_csv_path = PROJECT_DIR + string("/hfsf_example_raydata.csv");

    std::ifstream csv_file(ground_csv_path);
    vector<vector<string>> ground_raydata = split_csv(ground_csv_path);

    // Create Simuluation Data
    SimulationData sd;

    // Constants
    const uint_fast64_t NRAYS = 10000;
    const double TOL = 1e-4;

    // Read Input File
    bool success = sd.import_from_file(sample_path);
    EXPECT_TRUE(success);
    EXPECT_TRUE(sd.get_number_of_elements() > 0);
    EXPECT_TRUE(sd.get_number_of_ray_sources() > 0);

    // Parameters
    SimulationParameters &params = sd.get_simulation_parameters();
    params.include_optical_errors = false;
    params.include_sun_shape_errors = false;
    params.max_number_of_rays = NRAYS * 100;
    params.number_of_rays = NRAYS;
    params.seed = 1;

    // Run Ray Trace
    NativeRunner runner;
    runner.disable_point_focus();
    runner.disable_power_tower();
    int sts;
    sts = runner.initialize();
    EXPECT_EQ(sts, RunnerStatus::SUCCESS);
    sts = runner.setup_simulation(&sd);
    EXPECT_EQ(sts, RunnerStatus::SUCCESS);

    auto stage_list = runner.get_system()->StageList;
    tstage_ptr tstage = nullptr;
    for (auto iter = stage_list.begin();
         iter != stage_list.end();
         ++iter)
    {
        tstage = *iter;
        tstage->MultiHitsPerRay = false;
        tstage->TraceThrough = false;
    }

    sts = runner.run_simulation();
    EXPECT_EQ(sts, RunnerStatus::SUCCESS);

    const TSystem *sys = runner.get_system();
    const TRayData *ray_data = &(sys->AllRayData);
    size_t nrdata = ray_data->Count();

    Vector3d point, cosines;
    int element;
    int stage;
    unsigned int raynum;

    // Retrieving data from Runner
    for (size_t i = 0; i < nrdata; i++)
    {
        EXPECT_TRUE(ray_data->Query(i, point.data, cosines.data,
                                    &element, &stage, &raynum));

        // element = abs(element);

        if (stage == 3)
        {
            // TODO: Stage 3 is virtual in the input file but
            // that has not been implemented yet. Remove after
            // implementing this.
            continue;
        }

        EXPECT_EQ(element, stoi(ground_raydata[6][i + 1]));
        EXPECT_EQ(stage, stoi(ground_raydata[7][i + 1]));
        EXPECT_EQ(raynum, stoul(ground_raydata[8][i + 1]));

        if (element == stoi(ground_raydata[6][i + 1]) &&
            stage == stoi(ground_raydata[7][i + 1]) &&
            raynum == stoul(ground_raydata[8][i + 1]))
        {
            EXPECT_NEAR(point[0], stod(ground_raydata[0][i + 1]), TOL);
            EXPECT_NEAR(point[1], stod(ground_raydata[1][i + 1]), TOL);
            EXPECT_NEAR(point[2], stod(ground_raydata[2][i + 1]), TOL);

            EXPECT_NEAR(cosines[0], stod(ground_raydata[3][i + 1]), TOL);
            EXPECT_NEAR(cosines[1], stod(ground_raydata[4][i + 1]), TOL);
            EXPECT_NEAR(cosines[2], stod(ground_raydata[5][i + 1]), TOL);
        }
        else
        {
            std::cout << "Line: " << i
                      << "\nElement: " << element
                      << "\nStage: " << stage
                      << "\nRay Number: " << raynum
                      << std::endl;
            break;
        }
    }
}
