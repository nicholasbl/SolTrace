Project(EmbreeRunnerUnitTests)

include_directories(
    .
    ../../common
    ../../../test_tools
    ../../../app/deploy/samples
    ../../../../coretrace/simulation_data
    ../../../../coretrace/simulation_results
    ../../../../coretrace/simulation_runner
    ../../../../coretrace/simulation_runner/native_runner
    ../../../../coretrace/simulation_runner/embree_runner
)

# Add test files for Embree_runner
set(EMBREE_RUNNER_TEST_SRC
    ../../common/common.cpp
    # flat_calculator_test.cpp
    # newton_calculator_test.cpp
    # parabola_calculator_test.cpp
    # sphere_calculator_test.cpp
    # cylinder_calculator_test.cpp
    # calculator_factory_test.cpp
    embree_runner_test.cpp
    # tower_demo.cpp
)

# if (NOT ENABLE_COVERAGE)
#     list(APPEND
#     EMBREE_RUNNER_TEST_SRC
#     embree_runner_multithreading_test.cpp
#     )
# endif()

add_executable(EmbreeRunnerUnitTests
    ${EMBREE_RUNNER_TEST_SRC}
)

target_link_libraries(
    EmbreeRunnerUnitTests
    simdata
    simresult
    # native_runner
    embree_runner
    testtools
    GTest::gtest_main
)

if (ENABLE_COVERAGE AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_options(EmbreeRunnerUnitTests PRIVATE --coverage)
    target_link_libraries(EmbreeRunnerUnitTests gcov)
endif()

if(UNIX)
    target_link_libraries(EmbreeRunnerUnitTests pthread)
endif()

add_test(NAME EmbreeRunnerUnitTests COMMAND EmbreeRunnerUnitTests)
