Project(native_runner)

include_directories(
    .
    ../../simulation_data
    ../../simulation_results
    ../../simulation_runner
)

set(NATIVE_RUNNER_SRC
    calculator_factory.cpp
    cylinder_calculator.cpp
    determine_element_intersection_new.cpp
    flat_calculator.cpp
    native_runner.cpp
    native_runner_types.cpp
    newton_calculator.cpp
    parabola_calculator.cpp
    pt_optimizations.cpp
    sphere_calculator.cpp
    sun_to_primary_stage.cpp
    trace.cpp
    treemesh.cpp
)

set(POSITION_INDEPENDENT_CODE ON)

if(WIN32)
    # Hack to build on Windows
    add_library(native_runner STATIC ${NATIVE_RUNNER_SRC})
else(WIN32)
    add_library(native_runner SHARED ${NATIVE_RUNNER_SRC})
endif()

set_target_properties(
    native_runner
    PROPERTIES
    DEBUG_POSTFIX "d"
	PREFIX ""
)

target_link_libraries(
    native_runner
    simdata
)

if (NOT APPLE AND ENABLE_COVERAGE)
	# SET(CMAKE_CXX_FLAGS "-O0 -coverage -fkeep-inline-functions")
    SET(CMAKE_CXX_FLAGS "-O0 -coverage")
endif()
