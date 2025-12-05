cmake_minimum_required(VERSION 3.16)

project(SolTraceApp VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.9 REQUIRED COMPONENTS Core Gui Quick Quick3D)


set(QT_QML_GENERATE_QMLLS_INI ON)

qt_standard_project_setup(REQUIRES 6.9)

qt_add_executable(SolTrace
    src/main.cpp
)

qt_add_qml_module(SolTrace
    URI SolTraceProto
    VERSION 1.0
    QML_FILES
        ui/Main.qml
        ui/TransparentPane.qml
    DEPENDENCIES
        QtQuick
        QtQuick3D
    SOURCES
        src/instance_table.h src/instance_table.cpp
        src/backend.h src/backend.cpp
        src/qt_helpers.h
        src/dataset.h src/dataset.cpp
        src/indirect_model.h src/indirect_model.cpp
        src/surfacegeometry.h src/surfacegeometry.cpp
        src/surfacegeometrylistmodel.h src/surfacegeometrylistmodel.cpp
        src/elementitemmodel.h src/elementitemmodel.cpp
        src/job_run.h src/job_run.cpp
        src/utility.h
)

qt_add_resources(SolTrace
    "assets"
    PREFIX
        /
    FILES
        assets/b_to_r_wide.png
)

target_include_directories(SolTrace PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_include_directories(SolTrace PRIVATE ../coretrace/simulation_data)
target_include_directories(SolTrace PRIVATE ../coretrace/simulation_results)
target_include_directories(SolTrace PRIVATE ../coretrace/simulation_runner)

set_target_properties(SolTrace PROPERTIES CXX_STANDARD 20)

set_target_properties(SolTrace PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER "gov.nrel.SolTrace"
    MACOSX_BUNDLE_BUNDLE_NAME "SolTrace"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(SolTrace
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Quick
        Qt6::Quick3D
        simdata
        simresult
        native_runner
)


set_target_properties(simdata PROPERTIES
    MACOSX_RPATH ON
    INSTALL_NAME_DIR "@rpath")


#set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if(APPLE)
  install(TARGETS simdata simresult native_runner
      LIBRARY DESTINATION "SolTrace.app/Contents/Frameworks")
elseif(WIN32)
  install(TARGETS simdata simresult native_runner
      RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}") # .dll next to .exe
else()
  install(TARGETS simdata simresult native_runner
      LIBRARY DESTINATION "${CMAKE_INSTALL_BINDIR}") # .so next to binary
endif()


include(GNUInstallDirs)
install(TARGETS SolTrace
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)


qt_generate_deploy_qml_app_script(
    TARGET SolTrace
    OUTPUT_SCRIPT deploy_script
    MACOS_BUNDLE_POST_BUILD
    NO_UNSUPPORTED_PLATFORM_ERROR
    DEPLOY_USER_QML_MODULES_ON_UNSUPPORTED_PLATFORM
)
install(SCRIPT ${deploy_script})
