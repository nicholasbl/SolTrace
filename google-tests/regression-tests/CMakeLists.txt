
Project(RegressionTests)

include_directories(
	.
	# ../app/deploy/samples
	../../coretrace/simulation_data
	../../coretrace/simulation_data/cst_templates
	../../coretrace/simulation_results
	../../coretrace/simulation_runner
	../../coretrace/simulation_runner/native_runner
)

# file(GLOB UNIT_TEST_SRC *.cpp)

# Add test files below as needed. Add more basic ones earlier in the list
# so those tests run prior to the more complicated ones.
# common.hpp and common.cpp contain helper methods used in multiple files.
set(REGRESSION_TEST_SRC
	native_runner_performance_test.cpp
)

add_executable(RegressionTests
    ${REGRESSION_TEST_SRC}
)

target_link_libraries(
    RegressionTests
    simdata
	simresult
	native_runner
	GTest::gtest_main
)

if (ENABLE_COVERAGE AND CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_link_options(UnitTests PRIVATE --coverage)
	target_link_libraries(UnitTests gcov)
# else()
# 	target_link_libraries(UnitTests coretrace GTest::gtest_main)
endif()

if(UNIX)
	target_link_libraries(UnitTests pthread)
endif()
