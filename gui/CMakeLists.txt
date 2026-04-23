cmake_minimum_required(VERSION 3.16)

project(SolTraceApp VERSION 0.1 LANGUAGES CXX)

include(cmake/CPM.cmake)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.11 REQUIRED COMPONENTS Core Gui Quick Quick3D Widgets Graphs)

set(QT_QML_GENERATE_QMLLS_INI ON)

qt_standard_project_setup(REQUIRES 6.11)

qt_add_executable(SolTrace
    src/main.cpp
)

# Add all QML files

file(GLOB_RECURSE SOLTRACE_QML_FILES
    CONFIGURE_DEPENDS
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "ui/*.qml"
)
list(SORT SOLTRACE_QML_FILES)

# Add all cpp files

file(GLOB_RECURSE SOLTRACE_CPP_FILES
    CONFIGURE_DEPENDS
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "src/*.cpp"
    "src/*.h"
)
list(REMOVE_ITEM SOLTRACE_CPP_FILES "src/main.cpp")
list(SORT SOLTRACE_CPP_FILES)

# TODO tighten bounds on files
file(GLOB_RECURSE SOLTRACE_ASSET_FILES
    CONFIGURE_DEPENDS
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "assets/*"
)
list(FILTER SOLTRACE_ASSET_FILES EXCLUDE REGEX "/\\.DS_Store$")
list(SORT SOLTRACE_ASSET_FILES)

# Add all documents
file(GLOB_RECURSE SOLTRACE_DOC_FILES
    CONFIGURE_DEPENDS
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "docs/*.md"
)
list(SORT SOLTRACE_DOC_FILES)

set_source_files_properties(ui/App.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_resources(SolTrace "assets"
    PREFIX "/"
    FILES
        ${SOLTRACE_ASSET_FILES}
)

qt_add_resources(SolTrace "docs"
    PREFIX "/"
    FILES
        ${SOLTRACE_DOC_FILES}
)

qt_add_qml_module(SolTrace
    URI SolTrace
    VERSION 1.0

    QML_FILES
        ${SOLTRACE_QML_FILES}

    SOURCES
        ${SOLTRACE_CPP_FILES}

    DEPENDENCIES
        QtQuick
        QtQuick3D
        SOURCES src/analysis/ray_volume_raster.cpp
        QML_FILES ui/panels/analysis/AnalyzeIntersections.qml
        QML_FILES ui/panels/analysis/RayFilterPopup.qml
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
    INSTALL_NAME_DIR "@rpath"
)

if(APPLE)
    install(TARGETS simdata simresult native_runner
        LIBRARY DESTINATION "SolTrace.app/Contents/Frameworks"
    )
elseif(WIN32)
    install(TARGETS simdata simresult native_runner
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    )
else()
    install(TARGETS simdata simresult native_runner
        LIBRARY DESTINATION "${CMAKE_INSTALL_BINDIR}"
    )
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
