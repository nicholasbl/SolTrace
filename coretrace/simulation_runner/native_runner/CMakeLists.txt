Project(native_runner)

include_directories(
    .
    # ${CMAKE_SOURCE_DIR}/coretrace
    ${CMAKE_SOURCE_DIR}/coretrace/simulation_data
    ${CMAKE_SOURCE_DIR}/coretrace/simulation_runner
)

set(NATIVE_RUNNER_SRC
    # ${CMAKE_SOURCE_DIR}/coretrace/treemesh.cpp
    # ${CMAKE_SOURCE_DIR}/coretrace/types.cpp
    calculator_factory.cpp
    determine_element_intersection_new.cpp
    # element_intersection.cpp
    native_runner.cpp
    native_runner_types.cpp
    parabola_calculator.cpp
    pt_optimizations.cpp
    # quadric_calculator.cpp
    # root432.cpp
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
