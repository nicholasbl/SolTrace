Project(CSTTests)

include_directories(
	.
	../
	../../app/deploy/samples
	../../../coretrace/simulation_data
	../../../coretrace/simulation_results
	../../../coretrace/simulation_runner
	../../../coretrace/simulation_runner/native_runner
)

# CST Template test files
set(CST_TEST_SRC
	linear_fresnel_test.cpp
	parabolic_trough_test.cpp
	parabolic_dish_test.cpp
	heliostat_test.cpp
)

add_executable(CSTTests
	${CST_TEST_SRC}
)

target_link_libraries(
	CSTTests
	simdata
	native_runner
	GTest::gtest_main
)

if (ENABLE_COVERAGE AND CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_link_options(CSTTests PRIVATE --coverage)
	target_link_libraries(CSTTests gcov)
endif()

if(UNIX)
	target_link_libraries(CSTTests pthread)
endif()
