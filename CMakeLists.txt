cmake_minimum_required(VERSION 3.19)

Project(soltrace_ui VERSION 1.0.0)

include(cmake/CPM.cmake)

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
option(SOLTRACE_ENABLE_ASAN "Enable AddressSanitizer for supported builds" OFF)

option(FULL_FIELD_VALIDATION_TEST "Include full field validation tests" OFF)

if(SOLTRACE_ENABLE_ASAN)
	if(MSVC)
		message(WARNING "SOLTRACE_ENABLE_ASAN is not supported with MSVC")
	elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
		add_compile_options(
			$<$<COMPILE_LANGUAGE:C,CXX>:-fsanitize=address>
			$<$<COMPILE_LANGUAGE:C,CXX>:-fno-omit-frame-pointer>
		)
		add_link_options(-fsanitize=address)
	else()
		message(WARNING "SOLTRACE_ENABLE_ASAN is only supported with Clang and GCC")
	endif()
endif()


enable_testing()

add_subdirectory(coretrace)
add_subdirectory(google-tests)

if(SOLTRACE_BUILD_GUI)
	add_subdirectory(app)
endif()
