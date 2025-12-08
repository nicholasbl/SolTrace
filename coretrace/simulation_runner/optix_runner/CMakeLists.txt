Project(optix_runner)

set(OPTIX_RUNNER_SRC
    optix_runner.cpp
)

set(POSITION_INDEPENDENT_CODE ON)

if(WIN32)
    # Hack to build on Windows
    add_library(optix_runner STATIC ${OPTIX_RUNNER_SRC})
else(WIN32)
    add_library(optix_runner SHARED ${OPTIX_RUNNER_SRC})
endif()

set_target_properties(
    optix_runner
    PROPERTIES
    DEBUG_POSTFIX "d"
	PREFIX ""
)

# Publish include directories so dependents inherit them
target_include_directories(optix_runner
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}                           # optix_runner headers
        ${CMAKE_CURRENT_SOURCE_DIR}/../..                     # coretrace root
        ${CMAKE_CURRENT_SOURCE_DIR}/../../simulation_data
        ${CMAKE_CURRENT_SOURCE_DIR}/../../simulation_runner
        ${CMAKE_CURRENT_SOURCE_DIR}/../../simulation_results
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../app/deploy/samples  # contains soltrace_system.h
)

target_link_libraries(optix_runner
    PUBLIC  OptixCSP::OptixCSP_core
            CUDA::cuda_driver
            simdata
)