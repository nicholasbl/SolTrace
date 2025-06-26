cmake_minimum_required(VERSION 3.19)

Project(soltrace_ui VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(SOLTRACE_BUILD_GUI "Build the GUI parts of Soltrace" ON)

enable_testing()

add_subdirectory(coretrace)
add_subdirectory(google-tests)

if(SOLTRACE_BUILD_GUI)
	add_subdirectory(app)
endif()

