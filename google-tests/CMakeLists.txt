
include(FetchContent)
FetchContent_Declare(
	googletest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG main
)

include_directories(soltrace_example_tests csv_tools ../coretrace ../coretrace/strace ../app/deploy/samples)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


file(GLOB SAMPLE_FILES ./samples/*.stinput)
file(GLOB GROUND_FILES ./ground_results/*.csv)
file(GLOB SOLTRACE_EXAMPLE_TESTS soltrace_example_tests/*.cpp)
file(GLOB CSV_TOOL csv_tools/*.cpp csv_tools/*.h)

set(STRACE_SRC ../coretrace/strace)
set(CORETRACE ../coretrace/stapi.cpp)
set(STRACE ../coretrace/strace/strace.cpp)
set(ENV{SOLTRACE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}")


file(COPY ${SAMPLE_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${GROUND_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

message("Binary dir: ${CMAKE_CURRENT_BINARY_DIR}")
message("Source dir: ${CMAKE_CURRENT_SOURCE_DIR}")
message("Build type: ${CMAKE_BUILD_TYPE}")

add_executable( TestExecutable
	${CSV_TOOL}
	${SOLTRACE_EXAMPLE_TESTS}
	${CORETRACE}
	${STRACE}
)

target_link_libraries(
	TestExecutable
	coretrace
	GTest::gtest_main
)

if(UNIX)
	target_link_libraries(TestExecutable pthread)
endif()


add_definitions(-DPROJECT_DIR="${CMAKE_CURRENT_BINARY_DIR}")

enable_testing()
add_test(NAME Test COMMAND TestExecutable)

if(MSVC)
	set_tests_properties(Test PROPERTIES WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()
