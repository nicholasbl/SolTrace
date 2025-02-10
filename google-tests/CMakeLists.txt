
include(FetchContent)
FetchContent_Declare(
	googletest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG main
)

include_directories(soltrace_example_tests hello_world ../coretrace ../coretrace/strace ../app/deploy/samples)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


file(GLOB SAMPLE_FILES ./samples/*.stinput)
file(GLOB SOLTRACE_EXAMPLE_TESTS soltrace_example_tests/st_mtrand_test.cpp)

set(STRACE_SRC ../coretrace/strace)

set(CORETRACE ../coretrace/stapi.cpp)
set(STRACE ../coretrace/strace/strace.cpp)
set(ENV{SOLTRACE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}")

# Copying files to build directory on building seems to be a common practice in cmake.
# I wanted to use configure_file to copy these files, but this command seems to have issues with relative paths 
#(such as the ones created when globbing the sample files together)

#configure_file(${SAMPLE_FILES} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)


file(COPY ${SAMPLE_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
#if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/Release)
#	file(COPY ${SAMPLE_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release)
#endif()
#if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/Debug)
	#file(COPY ${SAMPLE_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
#endif()

message("Binary dir: ${CMAKE_CURRENT_BINARY_DIR}")
message("Source dir: ${CMAKE_CURRENT_SOURCE_DIR}")
message("Build type: ${CMAKE_BUILD_TYPE}")





add_executable( TestExecutable
	${SOLTRACE_EXAMPLE_TESTS}
	${CORETRACE}
	${STRACE}
)

#add_test(NAME TestTest COMMAND Test)
#set_tests_properties(TestTest PROPERTIES WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

add_compile_definitions(STRACE_SRC=${STRACE_SRC})
add_compile_definitions(SAMPLE_FILES=${SAMPLE_FILES})

#add_definitions( -D_SAMPLES = ${SAMPLE_FILES})

target_link_libraries(
	TestExecutable
	coretrace
	GTest::gtest_main
)

if(UNIX)
	target_link_libraries(TestExecutable pthread)
endif()

#set_target_properties(Test PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

#include(GoogleTest)
#gtest_discover_tests(Test)

add_definitions(-DPROJECT_DIR="${CMAKE_CURRENT_BINARY_DIR}")

enable_testing()
add_test(NAME Test COMMAND TestExecutable)

if(MSVC)
	set_tests_properties(Test PROPERTIES WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()
