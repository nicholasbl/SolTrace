
Project(UnitTests)

include_directories(
	.
	../app/deploy/samples
	../../coretrace
	../../coretrace/simulation_data
	../../coretrace/simulation_results
	../../coretrace/simulation_runner
	../../coretrace/simulation_runner/native_runner
)

# Add CST template tests subdirectory
add_subdirectory(cst-templates)

# file(GLOB UNIT_TEST_SRC *.cpp)

# Add test files below as needed. Add more basic ones earlier in the list
# so those tests run prior to the more complicated ones.
# common.hpp and common.cpp contain helper methods used in multiple files.
set(UNIT_TEST_SRC
    common.cpp
    linear_algebra_test.cpp
	aperture_test.cpp
	surface_test.cpp
    container_test.cpp
    element_test.cpp
    simulation_data_test.cpp
	flat_calculator_test.cpp
	newton_calculator_test.cpp
	parabola_calculator_test.cpp
	sphere_calculator_test.cpp
	cylinder_calculator_test.cpp
	calculator_factory_test.cpp
	native_runner_test.cpp
	tower_demo.cpp
)

add_executable(UnitTests
    ${UNIT_TEST_SRC}
)

target_link_libraries(
    UnitTests
    simdata
	native_runner
	coretrace
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
