Project(native_runner)

include_directories(
    .
    # ${CMAKE_SOURCE_DIR}/coretrace
    ${CMAKE_SOURCE_DIR}/coretrace/simulation_runner
    ${CMAKE_SOURCE_DIR}/coretrace/simulation_data
)

set(NATIVE_RUNNER_SRC
    # ${CMAKE_SOURCE_DIR}/coretrace/treemesh.cpp
    # ${CMAKE_SOURCE_DIR}/coretrace/types.cpp
    native_runner.cpp
    native_runner_types.cpp
    trace.cpp
    treemesh.cpp
)

set(POSITION_INDEPENDENT_CODE ON)

add_library(native_runner STATIC ${NATIVE_RUNNER_SRC})
set_target_properties(
    native_runner
    PROPERTIES
    DEBUG_POSTFIX "d"
	PREFIX ""
)

if (NOT APPLE AND ENABLE_COVERAGE)
	SET(CMAKE_CXX_FLAGS "-O0 -coverage -fkeep-inline-functions")
endif()
