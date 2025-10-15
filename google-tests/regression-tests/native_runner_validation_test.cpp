#include <gtest/gtest.h>

#include <sstream>

#include <native_runner.hpp>
#include <simulation_data_export.hpp>
#include <simulation_result_export.hpp>

#include "split_csv.h"

using namespace std;

using SolTrace::Runner::RunnerStatus;

using SolTrace::NativeRunner::NativeRunner;
using SolTrace::NativeRunner::TRayData;
using SolTrace::NativeRunner::tstage_ptr;
using SolTrace::NativeRunner::TSystem;

using SolTrace::Result::RayEvent;

bool get_runner_element_and_stage(const NativeRunner *runner,
                                  element_id id,
                                  int_fast64_t &re,
                                  int_fast64_t &rs)
{
    // Assumes that `id` is a unique identifier and that an element
    // can belong only to a single stage
    bool found = false;
    const TSystem *sys = runner->get_system();
    for (unsigned stage_idx = 0;
         stage_idx < sys->StageList.size();
         ++stage_idx)
    {
        tstage_ptr stage = sys->StageList[stage_idx];
        for (unsigned element_idx = 0;
             element_idx < stage->ElementList.size();
             ++element_idx)
        {
            SolTrace::NativeRunner::telement_ptr tel =
                stage->ElementList[element_idx];
            if (tel->sim_data_id == id)
            {
                // NativeRunner stage and element ids are 1 based
                // so we add 1 to the vector index here.
                found = true;
                re = element_idx + 1;
                rs = stage_idx + 1;
            }
        }

        if (found)
            break;
    }

    return found;
}

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
    RunnerStatus sts;
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

    // const TSystem *sys = runner.get_system();
    // const TRayData *ray_data = &(runner.get_system()->RayData);
    // size_t nrdata = ray_data->Count();

    // ray_data->Print();

    SimulationResult result;
    sts = runner.report_simulation(&result, 0);
    EXPECT_EQ(sts, RunnerStatus::SUCCESS);
    EXPECT_EQ(result.get_number_of_records(), NRAYS);

    Vector3d point, cosines;
    Vector3d pos_stage, dir_stage;
    Vector3d temp;
    int_fast64_t element;
    int_fast64_t stage;
    uint_fast64_t rayidx;
    uint_fast64_t iidx;
    element_ptr el = nullptr;
    int_fast64_t run_element, run_stage;

    // size_t total_lines = ground_raydata[0].size();
    size_t total_lines = std::numeric_limits<size_t>::max();
    size_t i;
    for (i = 0; i < 9; ++i)
    {
        total_lines = std::min(total_lines, ground_raydata[i].size());
    }

    // Compare saved CSV values to runner values
    for (i = 1; i < total_lines; ++i)
    {
        element = stoi(ground_raydata[6][i]);
        stage = stoi(ground_raydata[7][i]);
        // Legacy SolTrace and CSV file had 1-based ray IDs. SimulationResult
        // has 0-based ray ID's so subtract 1 here.
        rayidx = stoul(ground_raydata[8][i]) - 1;

        if (stage == 3)
        {
            // TODO: Stage 3 is virtual in the input file but
            // that has not been implemented yet. Remove after
            // implementing this.
            continue;
        }

        const ray_record_ptr rr = result[rayidx];
        if (element > 0)
        {
            bool found = false;
            for (uint_fast64_t idx = 0; idx < rr->get_number_of_interactions(); ++idx)
            {
                run_element = -1;
                run_stage = -1;
                bool sts = get_runner_element_and_stage(&runner,
                                                        rr->get_element(idx),
                                                        run_element,
                                                        run_stage);
                if (sts && element == run_element && stage == run_stage)
                {
                    iidx = idx;
                    found = true;
                    break;
                }
            }

            // assert(found);
            EXPECT_TRUE(found);

            if (found)
            {
                EXPECT_NE(rr->get_event(iidx), RayEvent::CREATE);
                EXPECT_NE(rr->get_event(iidx), RayEvent::ABSORB);
                EXPECT_NE(rr->get_event(iidx), RayEvent::EXIT);
            }
            else
            {
                std::cout << "CSV Line: " << i + 1
                          << "Ray Record: " << *rr
                          << std::endl;
                break;
            }

            rr->get_position(iidx, point);
            // Legacy SolTrace stored the incoming ray direction whereas
            // NativeRunner/SimulationResult stores the exit direction so
            // we take the direction for the previous ray event.
            rr->get_direction(iidx - 1, cosines);
        }
        else
        {
            iidx = rr->get_number_of_interactions() - 1;
            if (element == 0)
            {
                // Ray miss -- check only that it was noted
                EXPECT_EQ(rr->get_event(iidx), RayEvent::EXIT);
                continue;
            }
            else
            {
                EXPECT_EQ(rr->get_event(iidx), RayEvent::ABSORB);
                get_runner_element_and_stage(&runner,
                                             rr->get_element(iidx),
                                             run_element,
                                             run_stage);
                EXPECT_EQ(run_element, abs(element));
                EXPECT_EQ(run_stage, stage);

                rr->get_position(iidx, point);
                // Legacy SolTrace stored the incoming ray direction whereas
                // NativeRunner/SimulationResult stores the exit direction so
                // we take the direction for the previous ray event.
                rr->get_direction(iidx - 1, cosines);
            }
        }

        el = sd.get_element(rr->get_element(iidx));
        EXPECT_NE(el, nullptr);

        if (el == nullptr)
        {
            std::cout << "CSV Line: " << i + 1
                      << "\nRay Number: " << rayidx + 1
                      << "\nElement: " << rr->get_element(iidx)
                      << " CSV Element: " << element
                      << " Runner Element: " << run_element
                      << "\nCSV Stage: " << stage
                      << " Runner Stage: " << run_stage
                      << std::endl;
            break;
        }

        // Runner and SimulationResult store everything in global
        // coordinate whereas the CSV file is in stage coordinates
        // as per legacy SolTrace
        el->convert_global_to_reference(pos_stage, point);

        // See previous comment about coordinates
        el->convert_vector_global_to_reference(dir_stage, cosines);

        EXPECT_NEAR(pos_stage[0], stod(ground_raydata[0][i]), TOL);
        EXPECT_NEAR(pos_stage[1], stod(ground_raydata[1][i]), TOL);
        EXPECT_NEAR(pos_stage[2], stod(ground_raydata[2][i]), TOL);

        EXPECT_NEAR(dir_stage[0], stod(ground_raydata[3][i]), TOL);
        EXPECT_NEAR(dir_stage[1], stod(ground_raydata[4][i]), TOL);
        EXPECT_NEAR(dir_stage[2], stod(ground_raydata[5][i]), TOL);

        // if (fabs(pos_stage[0] - stod(ground_raydata[0][i]) > TOL))
        // {
        //     Vector3d pos_csv(stod(ground_raydata[0][i]),
        //                      stod(ground_raydata[1][i]),
        //                      stod(ground_raydata[2][i]));
        //     Vector3d dir_csv(stod(ground_raydata[3][i]),
        //                      stod(ground_raydata[4][i]),
        //                      stod(ground_raydata[5][i]));

        //     Vector3d pos_loc;
        //     Vector3d dir_loc;

        //     Vector3d csv_glob;
        //     el->convert_stage_to_local(temp, pos_csv);
        //     el->convert_local_to_global(csv_glob, temp);

        //     el->convert_global_to_local(pos_loc, point);
        //     el->convert_vector_global_to_local(dir_loc, cosines);

        //     std::cout << "CSV Line: " << i + 1
        //               << "\nRay Number: " << rayidx + 1
        //               << "\nElement: " << rr->get_element(iidx)
        //               << " CSV Element: " << element
        //               << " Runner Element: " << run_element
        //               << "\nCSV Stage: " << stage
        //               << " Runner Stage: " << run_stage
        //               << std::endl;

        //     std::cout << "CSV Pos: " << pos_csv
        //               << "\nCSV Global: " << csv_glob
        //               << "\nPos Global: " << point
        //               << "\nPos Stage: " << pos_stage
        //               << "\nPos Local: " << pos_loc
        //               << "\nCSV Dir: " << dir_csv
        //               << "\nDir Global: " << cosines
        //               << "\nDir Stage: " << dir_stage
        //               << "\nDir Local: " << dir_loc
        //               << std::endl;

        //     std::cout << "Ray Record: "
        //               << *rr
        //               << std::endl;

        //     std::cout << "Element Global Origin: " << el->get_origin_global()
        //               << "\nElement Local to Global: " << el->get_local_to_global()
        //               << "\nElement Ref to Local: " << el->get_local_to_reference()
        //               << std::endl;

        //     break;
        // }
    }
}
