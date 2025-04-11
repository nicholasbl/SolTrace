
Project(UnitTests)

include_directories(
	.
	../coretrace
	../app/deploy/samples
	../../coretrace/simulation_data
)

# file(GLOB UNIT_TEST_SRC *.cpp)

# Add test files below as needed. Add more basic ones earlier in the list
# so those tests run prior to the more complicated ones.
# common.hpp and common.cpp contain helper methods used in multiple files.
set(UNIT_TEST_SRC
    common.cpp
    linear_algebra_test.cpp
    container_test.cpp
    element_test.cpp
    simulation_data_test.cpp
)

add_executable(UnitTests
    ${UNIT_TEST_SRC}
)

target_link_libraries(
    UnitTests
    simdata
	coretrace
	GTest::gtest_main
)

if(UNIX)
	target_link_libraries(UnitTests pthread)
endif()
