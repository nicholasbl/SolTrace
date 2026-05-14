#include <gtest/gtest.h>

#include <limits>
#include <stdexcept>

#include <optix_runner.hpp>
#include <simulation_data.hpp>
#include <simulation_data_export.hpp>
#include <simulation_result_export.hpp>

using SolTrace::Runner::RunnerStatus;

// Reuse the two-plate scene defined in two_plate_test.cpp
void make_two_plate_sd(SimulationData& sd, element_ptr& plate1, element_ptr& plate2);

// ---------------------------------------------------------------------------
// set_batch_size / get_batch_size accessor tests (no GPU required)
// ---------------------------------------------------------------------------

TEST(OptixRunnerBatchSize, DefaultIsZero)
{
    OptixRunner runner;
    EXPECT_EQ(runner.get_batch_size(), 0u);
}

TEST(OptixRunnerBatchSize, SetAndGet)
{
    OptixRunner runner;
    runner.set_batch_size(500);
    EXPECT_EQ(runner.get_batch_size(), 500u);
}

TEST(OptixRunnerBatchSize, SetZeroRestoresDefault)
{
    OptixRunner runner;
    runner.set_batch_size(1000);
    runner.set_batch_size(0);
    EXPECT_EQ(runner.get_batch_size(), 0u);
}

TEST(OptixRunnerBatchSize, ThrowsOnOverflow)
{
    OptixRunner runner;
    const uint_fast64_t too_large =
        static_cast<uint_fast64_t>(std::numeric_limits<int>::max()) + 1ULL;
    EXPECT_THROW(runner.set_batch_size(too_large), std::out_of_range);
    // Value should be unchanged after the throw
    EXPECT_EQ(runner.get_batch_size(), 0u);
}

// ---------------------------------------------------------------------------
// Simulation correctness: batched run should yield the same hit count as the
// default single-batch run.
// ---------------------------------------------------------------------------

TEST(OptixRunnerBatchSize, BatchedRunMatchesHitCount)
{
    const int N_rays = 10000;

    // --- reference run (default single batch) ---
    SimulationData sd_ref;
    element_ptr p1_ref, p2_ref;
    make_two_plate_sd(sd_ref, p1_ref, p2_ref);
    sd_ref.get_simulation_parameters().number_of_rays = N_rays;
    sd_ref.get_simulation_parameters().max_number_of_rays = N_rays * 100;

    OptixRunner ref_runner;
    ASSERT_EQ(ref_runner.initialize(),           RunnerStatus::SUCCESS);
    ASSERT_EQ(ref_runner.setup_simulation(&sd_ref),  RunnerStatus::SUCCESS);
    ASSERT_EQ(ref_runner.run_simulation(),        RunnerStatus::SUCCESS);

    SimulationResult ref_result;
    ASSERT_EQ(ref_runner.report_simulation(&ref_result, 0), RunnerStatus::SUCCESS);
    const int ref_hits = ref_result.get_number_of_records();

    // --- batched run (batch_size = 1000, i.e. 10 iterations) ---
    SimulationData sd_batch;
    element_ptr p1_batch, p2_batch;
    make_two_plate_sd(sd_batch, p1_batch, p2_batch);
    sd_batch.get_simulation_parameters().number_of_rays = N_rays;
    sd_batch.get_simulation_parameters().max_number_of_rays = N_rays * 100;

    OptixRunner batch_runner;
    batch_runner.set_batch_size(1000);
    ASSERT_EQ(batch_runner.initialize(),              RunnerStatus::SUCCESS);
    ASSERT_EQ(batch_runner.setup_simulation(&sd_batch),   RunnerStatus::SUCCESS);
    ASSERT_EQ(batch_runner.run_simulation(),           RunnerStatus::SUCCESS);

    SimulationResult batch_result;
    ASSERT_EQ(batch_runner.report_simulation(&batch_result, 0), RunnerStatus::SUCCESS);
    const int batch_hits = batch_result.get_number_of_records();

    EXPECT_EQ(ref_hits, N_rays);
    EXPECT_EQ(batch_hits, N_rays);

    // Default (single-batch) run completes in one iteration; batched run needs more
    EXPECT_EQ(ref_runner.get_N_run_iterations(), 1u);
    EXPECT_GT(batch_runner.get_N_run_iterations(), 1u);
}

// ---------------------------------------------------------------------------
// Batch size smaller than total rays forces multiple iterations.
// ---------------------------------------------------------------------------

TEST(OptixRunnerBatchSize, SmallBatchMultipleIterations)
{
    const int N_rays = 5000;
    const int batch  = 500; // 10+ iterations needed

    SimulationData sd;
    element_ptr plate1, plate2;
    make_two_plate_sd(sd, plate1, plate2);
    sd.get_simulation_parameters().number_of_rays = N_rays;
    sd.get_simulation_parameters().max_number_of_rays = N_rays * 100;

    OptixRunner runner;
    runner.set_batch_size(batch);
    ASSERT_EQ(runner.initialize(),           RunnerStatus::SUCCESS);
    ASSERT_EQ(runner.setup_simulation(&sd),  RunnerStatus::SUCCESS);
    ASSERT_EQ(runner.run_simulation(),        RunnerStatus::SUCCESS);

    SimulationResult result;
    ASSERT_EQ(runner.report_simulation(&result, 0), RunnerStatus::SUCCESS);

    EXPECT_EQ(result.get_number_of_records(), N_rays);

    // With a small batch the runner must have generated at least N_rays sun rays
    EXPECT_GE(runner.get_N_sun_rays(), static_cast<uint_fast64_t>(N_rays));

    // Multiple iterations must have been needed
    EXPECT_GE(runner.get_N_run_iterations(), 10u);
}
