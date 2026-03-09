cmake_minimum_required(VERSION 3.19)

Project(soltrace_ui VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(SOLTRACE_BUILD_GUI "Build the GUI parts of Soltrace" OFF)
option(
	SOLTRACE_BUILD_CORETRACE 
	"Legacy ray tracing backend including the strace executable"
	OFF)

option(SOLTRACE_BUILD_EMBREE_SUPPORT "Build Embree support for ray tracing" OFF)
option(SOLTRACE_BUILD_OPTIX_SUPPORT "Build the OptiX support for ray tracing" OFF)

option(SOLTRACE_BUILD_PERF_TEST "Build performance tests (not built for debug)" OFF)

option(FULL_FIELD_VALIDATION_TEST "Include full field validation tests" OFF)


enable_testing()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

configure_package_config_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/SolTraceConfig.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/SolTraceConfig.cmake"
	INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/SolTrace"
)

write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/SolTraceConfigVersion.cmake"
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion
)

install(FILES
	"${CMAKE_CURRENT_BINARY_DIR}/SolTraceConfig.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/SolTraceConfigVersion.cmake"
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindOptiX.cmake"
	DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/SolTrace"
)

install(EXPORT SolTraceTargets
	FILE SolTraceTargets.cmake
	NAMESPACE SolTrace::
	DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/SolTrace"
)

add_subdirectory(coretrace)
add_subdirectory(google-tests)

if(SOLTRACE_BUILD_GUI)
	add_subdirectory(app)
endif()

