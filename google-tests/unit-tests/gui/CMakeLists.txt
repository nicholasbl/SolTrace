Project(GUIUnitTests)

find_package(Qt6 6.11 REQUIRED COMPONENTS Core)

set(GUI_UNIT_TEST_SRC
    ../../../gui/src/database/conversion.cpp
    conversion_test.cpp
)

add_executable(GUIUnitTests
    ${GUI_UNIT_TEST_SRC}
)

target_include_directories(GUIUnitTests
    PRIVATE
        ../../../gui/src
        ../../../coretrace/simulation_data
)

target_link_libraries(GUIUnitTests
    PRIVATE
        Qt6::Core
        simdata
        GTest::gtest_main
)

set_target_properties(GUIUnitTests PROPERTIES CXX_STANDARD 20)

if (ENABLE_COVERAGE AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_options(GUIUnitTests PRIVATE --coverage)
    target_link_libraries(GUIUnitTests PRIVATE gcov)
endif()

if(UNIX)
    target_link_libraries(GUIUnitTests PRIVATE pthread)
endif()

add_test(NAME GUIUnitTests COMMAND GUIUnitTests)
