Project(optix_runner)

add_subdirectory(OptixCSP)

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
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../simulation_data>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../simulation_runner>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../simulation_results>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../app/deploy/samples>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/OptixCSP/src>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/SolTrace/simulation_runner/optix_runner>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/SolTrace/simulation_runner>
)

target_link_libraries(optix_runner
    PUBLIC  OptixCSP::OptixCSP_core
            CUDA::cuda_driver
            simdata
)

install(TARGETS optix_runner
    EXPORT SolTraceTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/optix_runner.hpp"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/SolTrace/simulation_runner/optix_runner
)
