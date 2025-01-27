
include(FetchContent)
FetchContent_Declare(
	googletest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG main
)

include_directories(soltrace_example_tests hello_world ../coretrace ../coretrace/strace ../app/deploy/samples)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


file(GLOB SAMPLE_FILES ../app/deploy/samples/*.stinput)
file(GLOB SOLTRACE_EXAMPLE_TESTS soltrace_example_tests/st_sim_run_test.cpp)

set(STRACE_SRC ../coretrace/strace)

set(CORETRACE ../coretrace/stapi.cpp)
set(STRACE ../coretrace/strace/strace.cpp)
set(ENV{SOLTRACE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}")

# Copying files to build directory on building seems to be a common practice in cmake.
# I wanted to use configure_file to copy these files, but this command seems to have issues with relative paths 
#(such as the ones created when globbing the sample files together)

#configure_file(${SAMPLE_FILES} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
file(COPY ${SAMPLE_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)

message("Binary dir: ${CMAKE_CURRENT_BINARY_DIR}")
message("Source dir: ${CMAKE_CURRENT_SOURCE_DIR}")

enable_testing()



add_executable( Test
	${SOLTRACE_EXAMPLE_TESTS}
	${CORETRACE}
	${STRACE}
	${SAMPLE_FILES}
)

#add_test(NAME TestTest COMMAND Test)
#set_tests_properties(TestTest PROPERTIES WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

add_compile_definitions(STRACE_SRC=${STRACE_SRC})
add_compile_definitions(SAMPLE_FILES=${SAMPLE_FILES})

#add_definitions( -D_SAMPLES = ${SAMPLE_FILES})

target_link_libraries(
	Test
	coretrace
	GTest::gtest_main
)

set_target_properties(Test PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

include(GoogleTest)
gtest_discover_tests(Test)
