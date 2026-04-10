cmake_minimum_required(VERSION 3.19)

Project(soltrace_ui VERSION 1.0.0)

include(cmake/CPM.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(SOLTRACE_BUILD_GUI "Build the GUI parts of Soltrace" ON)
option(
	SOLTRACE_BUILD_CORETRACE 
	"Legacy ray tracing backend including the strace executable"
	OFF)

option(SOLTRACE_BUILD_EMBREE_SUPPORT "Build Embree support for ray tracing" OFF)
option(SOLTRACE_BUILD_OPTIX_SUPPORT "Build the OptiX support for ray tracing" OFF)

option(SOLTRACE_BUILD_PERF_TEST "Build performance tests (not built for debug)" OFF)

option(FULL_FIELD_VALIDATION_TEST "Include full field validation tests" OFF)


if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Enabling AddressSanitizer")
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
endif()

enable_testing()

add_subdirectory(coretrace)
add_subdirectory(google-tests)

if(SOLTRACE_BUILD_GUI)
	add_subdirectory(gui)
endif()

