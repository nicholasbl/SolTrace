#include <gtest/gtest.h>

#include <optix_runner.hpp>
#include <shaders/Soltrace.h>  // OptixCSP::DEFAULT_MAX_TRACE_DEPTH

// ---------------------------------------------------------------------------
// set_max_ray_depth / get_max_ray_depth accessor tests (no GPU required)
// ---------------------------------------------------------------------------

TEST(OptixRunnerMaxRayDepth, DefaultIsDefaultMaxTraceDepth)
{
    OptixRunner runner;
    EXPECT_EQ(runner.get_max_ray_depth(), OptixCSP::DEFAULT_MAX_TRACE_DEPTH);
}

TEST(OptixRunnerMaxRayDepth, SetAndGet)
{
    OptixRunner runner;
    runner.set_max_ray_depth(10);
    EXPECT_EQ(runner.get_max_ray_depth(), 10u);
}

// Minimum valid depth is 2; setting exactly 2 must be accepted without clamping.
TEST(OptixRunnerMaxRayDepth, MinimumBoundaryAccepted)
{
    OptixRunner runner;
    runner.set_max_ray_depth(2);
    EXPECT_EQ(runner.get_max_ray_depth(), 2u);
}

// Maximum valid depth is 255; setting exactly 255 must be accepted without clamping.
TEST(OptixRunnerMaxRayDepth, MaximumBoundaryAccepted)
{
    OptixRunner runner;
    runner.set_max_ray_depth(255);
    EXPECT_EQ(runner.get_max_ray_depth(), 255u);
}

// Depth of 0 is below the minimum; must be clamped to 2 with a warning.
TEST(OptixRunnerMaxRayDepth, ZeroClampsToMinimum)
{
    OptixRunner runner;
    testing::internal::CaptureStderr();
    runner.set_max_ray_depth(0);
    const std::string output = testing::internal::GetCapturedStderr();
    EXPECT_EQ(runner.get_max_ray_depth(), 2u);
    EXPECT_NE(output.find("WARNING"), std::string::npos);
}

// Depth of 1 is below the minimum; must be clamped to 2 with a warning.
TEST(OptixRunnerMaxRayDepth, OneClampsToMinimum)
{
    OptixRunner runner;
    testing::internal::CaptureStderr();
    runner.set_max_ray_depth(1);
    const std::string output = testing::internal::GetCapturedStderr();
    EXPECT_EQ(runner.get_max_ray_depth(), 2u);
    EXPECT_NE(output.find("WARNING"), std::string::npos);
}

// Depth of 256 exceeds the maximum; must be clamped to 255 with a warning.
TEST(OptixRunnerMaxRayDepth, ExceedingMaxClampsTo255)
{
    OptixRunner runner;
    testing::internal::CaptureStderr();
    runner.set_max_ray_depth(256);
    const std::string output = testing::internal::GetCapturedStderr();
    EXPECT_EQ(runner.get_max_ray_depth(), 255u);
    EXPECT_NE(output.find("WARNING"), std::string::npos);
}

// Large values (e.g. passing a big int) must also clamp to 255 with a warning.
TEST(OptixRunnerMaxRayDepth, LargeValueClampsTo255)
{
    OptixRunner runner;
    testing::internal::CaptureStderr();
    runner.set_max_ray_depth(10000);
    const std::string output = testing::internal::GetCapturedStderr();
    EXPECT_EQ(runner.get_max_ray_depth(), 255u);
    EXPECT_NE(output.find("WARNING"), std::string::npos);
}

// An out-of-range value is always clamped and stored, replacing any previous value.
TEST(OptixRunnerMaxRayDepth, ClampedValueOverwritesPreviousValue)
{
    OptixRunner runner;
    runner.set_max_ray_depth(20);
    ASSERT_EQ(runner.get_max_ray_depth(), 20u);

    testing::internal::CaptureStderr();
    runner.set_max_ray_depth(0);
    const std::string output = testing::internal::GetCapturedStderr();
    EXPECT_EQ(runner.get_max_ray_depth(), 2u);
    EXPECT_NE(output.find("WARNING"), std::string::npos);
}

// Setting a valid depth after an out-of-range call must work normally.
TEST(OptixRunnerMaxRayDepth, ValidSetAfterClamp)
{
    OptixRunner runner;
    testing::internal::CaptureStderr();
    runner.set_max_ray_depth(0);
    testing::internal::GetCapturedStderr();

    runner.set_max_ray_depth(8);
    EXPECT_EQ(runner.get_max_ray_depth(), 8u);
}
