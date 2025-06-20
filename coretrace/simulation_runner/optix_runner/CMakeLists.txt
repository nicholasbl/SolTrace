Project(optix_runner)

# need to find CUDA, optix, and optix_soltarce library

# before you define optix_runner
add_library(optix_soltrace STATIC IMPORTED)

set_target_properties(optix_soltrace PROPERTIES
    IMPORTED_LOCATION
        "C:/Users/fang/Documents/NREL_SOLAR/optix/interface_build/lib/Release/optix_soltrace.lib"
    INTERFACE_INCLUDE_DIRECTORIES
        "C:/Users/fang/Documents/NREL_SOLAR/optix/optixSoltraceDemos/interface;C:/Users/fang/Documents/NREL_SOLAR/optix/optixSoltraceDemos/lib;C:/Users/fang/Documents/NREL_SOLAR/optix/optixSoltraceDemos/cuda;C:/Users/fang/Documents/NREL_SOLAR/optix/optixSoltraceDemos;${CUDAToolkit_LIBRARY_ROOT}/include;${OptiX_ROOT_DIR}/include;${OptiX_ROOT_DIR}/SDK/;C:/Users/fang/Documents/NREL_SOLAR/optix/interface_build/"    # headers
)

message("OptiX_ROOT_DIR_SDK: ${OptiX_ROOT_DIR}/SDK")

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

target_link_libraries(
    optix_runner
    simdata
    optix_soltrace
)