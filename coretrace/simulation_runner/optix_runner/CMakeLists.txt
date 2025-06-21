Project(optix_runner)

find_package(CUDAToolkit REQUIRED)

# TODO: this part should not be hardcoded 
# add a way for user to specify the build path to OptixCSP
set(OptixCSP_DIR "/Users/fang/Documents/NREL_SOLAR/optix/cmake_refactor/build/cmake")
find_package(OptixCSP REQUIRED)

include_directories(
    .
    ${CMAKE_SOURCE_DIR}/coretrace
    ${CMAKE_SOURCE_DIR}/coretrace/simulation_data
    ${CMAKE_SOURCE_DIR}/coretrace/simulation_runner
)

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

target_link_libraries(optix_runner
    PUBLIC  OptixCSP::OptixCSP_core
            simdata
            CUDA::cuda_driver
)