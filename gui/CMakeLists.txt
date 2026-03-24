cmake_minimum_required(VERSION 3.16)

project(SolTraceApp VERSION 0.1 LANGUAGES CXX)

include(cmake/CPM.cmake)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.9 REQUIRED COMPONENTS Core Gui Quick Quick3D Widgets Graphs)

set(QT_QML_GENERATE_QMLLS_INI ON)

qt_standard_project_setup(REQUIRES 6.9)

qt_add_executable(SolTrace
    src/main.cpp
)

qt_add_resources(SolTrace "fonts"
    PREFIX "/"
    FILES
        assets/fonts/computer-modern/cmunrm.ttf
        assets/fonts/computer-modern/cmunbx.ttf
        assets/fonts/computer-modern/cmunti.ttf
        assets/fonts/computer-modern/cmunbi.ttf
        assets/fonts/computer-modern/cmunss.ttf
        assets/fonts/computer-modern/cmunsx.ttf
        assets/fonts/computer-modern/cmunsi.ttf
        assets/fonts/computer-modern/cmunso.ttf
        assets/fonts/roboto/Roboto-Bold.ttf
        assets/fonts/roboto/Roboto-BoldItalic.ttf
        assets/fonts/roboto/Roboto-Italic.ttf
        assets/fonts/roboto/Roboto-Regular.ttf
        assets/fonts/font-awesome/fa_solid_7.otf
)

qt_add_resources(SolTrace "images"
    PREFIX "/"
    FILES
        assets/images/mark.png
        assets/images/logo.png
        assets/images/logo.svg
        assets/images/directional_sun.svg
        assets/images/point_source.svg
        assets/images/directional_sun_eq.svg
        assets/images/point_source_sun_eq.svg
        assets/images/b_to_r_wide.png
)

qt_add_resources(SolTrace "icons"
    PREFIX "/"
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
        assets/icons/clock.svg
        assets/icons/collapse.svg
        assets/icons/dot.svg
        assets/icons/download.svg
        assets/icons/expand.svg
        assets/icons/files.svg
        assets/icons/import.svg
        assets/icons/list-checks.svg
        assets/icons/list-deselect.svg
        assets/icons/message-more.svg
        assets/icons/save.svg
        assets/icons/square-check.svg
        assets/icons/square-minus.svg
        assets/icons/stop.svg
        assets/icons/upload.svg
        assets/icons/messages.svg
        assets/icons/filter.svg
        assets/icons/swatch-book.svg
        assets/icons/sort-asc.svg
        assets/icons/sort-desc.svg
        assets/icons/book-marked.svg
        assets/icons/search.svg
        assets/icons/eye.svg
)


set_source_files_properties(ui/Theme.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_qml_module(SolTrace
    URI SolTrace
    VERSION 1.0
    QML_FILES
        ui/Main.qml
        ui/Theme.qml

        ui/3d/SimulationScene.qml

        ui/primitives/containers/GlassRectangle.qml
        ui/primitives/containers/ShadowedGlassRectangle.qml

        ui/primitives/widgets/STComboBar.qml
        ui/primitives/widgets/STIconButton.qml
        ui/primitives/widgets/STClickableLabel.qml
        ui/primitives/widgets/STTextField.qml
        ui/primitives/widgets/STPropertyLabel.qml
        ui/primitives/widgets/STPropertyPanel.qml

        ui/core/CorePanel.qml
        ui/core/ConfigureModule.qml
        ui/core/SimulateModule.qml
        ui/core/AnalysisModule.qml
        ui/core/TopBar.qml
        ui/core/ConfigureMaterials.qml
        ui/core/material/MaterialOpticals.qml


    DEPENDENCIES
        QtQuick
        QtQuick3D

    SOURCES
        src/backend.h src/backend.cpp
        src/utilities/qt_helpers.h
        src/utilities/indirect_model.h src/utilities/indirect_model.cpp
        src/job_control/job_run_thread.h
        src/job_control/job_run_process.cpp
        src/job_control/job_run_process.h
        src/job_control/job_run.h
        src/job_control/job_run.cpp
        src/job_control/job_run_thread.cpp
        src/job_control/job_run_common.h
        src/job_control/job_run_common.cpp
        src/utilities/math_utility.h
        src/utilities/structmodel.h
        src/utilities/notification.h
        src/database/database.h src/database/database.cpp
        src/database/database_models.h src/database/database_models.cpp
        src/database/rootelementsmodel.h src/database/rootelementsmodel.cpp
        src/database/components.h src/database/components.cpp
        src/database/geometryeditor.h src/database/geometryeditor.cpp
        src/database/database_notification.h src/database/database_notification.cpp
        src/analysis/grid3d.h
        src/analysis/ray_volume_raster.h
        src/analysis/ray_volume.h src/analysis/ray_volume.cpp
        src/analysis/ray_geometry.h src/analysis/ray_geometry.cpp
        src/database/worldgeometrymodel.h src/database/worldgeometrymodel.cpp
        src/database/conversion.h src/database/conversion.cpp
        src/database/opticaleditor.h src/database/opticaleditor.cpp
        src/app.h src/app.cpp
        src/modules/simulation_module.h src/modules/simulation_module.cpp
        src/modules/module_common.h src/modules/module_common.cpp
        src/modules/file_source_module.h src/modules/file_source_module.cpp
        src/modules/sun_module.h src/modules/sun_module.cpp
        src/modules/documentation_module.h src/modules/documentation_module.cpp
        src/modules/tracing_module.h src/modules/tracing_module.cpp
        src/modules/materials_module.h src/modules/materials_module.cpp
        src/modules/intersections_module.h src/modules/intersections_module.cpp
        src/modules/flux_module.h src/modules/flux_module.cpp
        src/modules/layout_module.h src/modules/layout_module.cpp
        src/modules/workflow_module.h src/modules/workflow_module.cpp
        QML_FILES ui/primitives/widgets/STSpinBox.qml
        QML_FILES ui/primitives/widgets/WellRectangle.qml
        QML_FILES ui/primitives/widgets/STComboBox.qml
        QML_FILES ui/core/material/SurfacePropertyGraphic.qml
        QML_FILES ui/core/ConfigureGeometry.qml
        QML_FILES ui/core/material/SurfacePreviewScene.qml
        QML_FILES ui/core/material/GeometryProperties.qml
        QML_FILES ui/3d/CustomOrbitController.qml
        QML_FILES ui/core/material/SurfaceGrid.qml
        SOURCES src/database/materialeditor.h src/database/materialeditor.cpp
        QML_FILES ui/core/ConfigureLayout.qml
        SOURCES src/database/apertureeditor.h src/database/apertureeditor.cpp
        SOURCES src/database/surfaceeditor.h src/database/surfaceeditor.cpp
        QML_FILES ui/core/element/InstanceEdit.qml



)

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

CPMAddPackage(
    NAME magic_enum
    GITHUB_REPOSITORY Neargye/magic_enum
    GIT_TAG v0.9.7
)

target_link_libraries(SolTrace PRIVATE EnTT magic_enum)

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
        Qt6::Widgets
        Qt6::Graphs
        simdata
        simresult
        native_runner
)

set_target_properties(simdata PROPERTIES
    MACOSX_RPATH ON
    INSTALL_NAME_DIR "@rpath")

if(APPLE)
  install(TARGETS simdata simresult native_runner
      LIBRARY DESTINATION "SolTrace.app/Contents/Frameworks")
elseif(WIN32)
  install(TARGETS simdata simresult native_runner
      RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
else()
  install(TARGETS simdata simresult native_runner
      LIBRARY DESTINATION "${CMAKE_INSTALL_BINDIR}")
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
