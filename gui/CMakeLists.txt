cmake_minimum_required(VERSION 3.16)

project(SolTraceApp VERSION 0.1 LANGUAGES CXX)

include(cmake/CPM.cmake)

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
        ui/WorldView.qml
        ui/TopBar.qml

        ui/components/GlassRect.qml
        ui/components/IconButton.qml
        ui/components/Callout.qml
        ui/components/GlassCombo.qml
        ui/components/ComboButtonGroup.qml
        ui/components/BrowserTreeNode.qml

        ui/viewports/SimScene.qml
        ui/viewports/SceneNav.qml

        ui/sections/Topbar.qml
        ui/sections/Configure.qml
        ui/sections/Simulate.qml
        ui/sections/Analyze.qml
        ui/sections/ContentPane.qml
    DEPENDENCIES
        QtQuick
        QtQuick3D
    SOURCES
        src/instance_table.h src/instance_table.cpp
        src/backend.h src/backend.cpp
        src/qt_helpers.h
        src/dataset.h src/dataset.cpp
        src/utilities/indirect_model.h src/utilities/indirect_model.cpp
        src/surfacegeometry.h src/surfacegeometry.cpp
        src/elementitemmodel.h src/elementitemmodel.cpp
        src/job_run.h src/job_run.cpp
        src/job_run_thread.h src/job_run_thread.cpp
        src/job_run_common.h
        src/job_run_process.h src/job_run_process.cpp
        src/utilities/math_utility.h
        src/utilities/structmodel.h
        src/models/geometrydefinitionsmodel.h src/models/geometrydefinitionsmodel.cpp
        src/database/database.h src/database/database.cpp
        src/database/database_models.h src/database/database_models.cpp
)

qt_add_resources(SolTrace
    "assets"
    PREFIX
        /
    FILES
        assets/b_to_r_wide.png
)

qt_add_resources(SolTrace "fonts"
    PREFIX "/fonts"
    FILES
        assets/fonts/computer-modern/cmunrm.ttf
        assets/fonts/computer-modern/cmunbx.ttf
        assets/fonts/computer-modern/cmunti.ttf
        assets/fonts/computer-modern/cmunbi.ttf
        assets/fonts/computer-modern/cmunss.ttf
        assets/fonts/computer-modern/cmunsx.ttf
        assets/fonts/computer-modern/cmunsi.ttf
        assets/fonts/computer-modern/cmunso.ttf
)

qt_add_resources(SolTrace "images"
    PREFIX "/images"
    FILES
        assets/images/mark.png
        assets/images/logo.png
        assets/images/logo.svg
)

qt_add_resources(SolTrace "icons"
    PREFIX "/icons"
    FILES
        assets/icons/chart.svg
        assets/icons/play.svg
        assets/icons/sliders.svg
        assets/icons/github.svg
        assets/icons/settings.svg
        assets/icons/blocks.svg
        assets/icons/box.svg
        assets/icons/chevron-down.svg
        assets/icons/chevron-right.svg
        assets/icons/info.svg
        assets/icons/list-video.svg
        assets/icons/placeholder.svg
        assets/icons/play-box.svg
        assets/icons/shapes.svg
        assets/icons/user.svg
        assets/icons/double-chevron-right.svg
        assets/icons/sun.svg
)

# Deps

CPMAddPackage(
  NAME EnTT
  VERSION 3.16.0
  GITHUB_REPOSITORY skypjack/entt
  DOWNLOAD_ONLY True
)

if(EnTT_ADDED)
  add_library(EnTT INTERFACE)
  target_include_directories(EnTT SYSTEM INTERFACE ${EnTT_SOURCE_DIR}/src)
endif()

target_link_libraries(SolTrace PRIVATE EnTT)


target_include_directories(SolTrace PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_include_directories(SolTrace PRIVATE ../coretrace/simulation_data)
target_include_directories(SolTrace PRIVATE ../coretrace/simulation_results)
target_include_directories(SolTrace PRIVATE ../coretrace/simulation_runner)

set_target_properties(SolTrace PROPERTIES CXX_STANDARD 20)

set_target_properties(SolTrace PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER "gov.nlr.SolTrace"
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
