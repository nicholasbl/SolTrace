cmake_minimum_required(VERSION 3.16)

project(SolTraceProto VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.9 REQUIRED COMPONENTS Core Gui Quick Quick3D)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)

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
        src/dataset.h
        SOURCES src/dataset.cpp
        SOURCES src/indirect_model.h src/indirect_model.cpp
        SOURCES src/surfacegeometry.h src/surfacegeometry.cpp
)

target_include_directories(SolTrace PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_include_directories(SolTrace PRIVATE ../coretrace/simulation_data)

set_target_properties(SolTrace PROPERTIES CXX_STANDARD 20)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(SolTrace PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.SolTrace
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(SolTrace
    PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Quick
        Qt::Quick3D
        simdata

)
target_link_libraries(SolTrace PRIVATE Qt6::Core)
target_link_libraries(SolTrace PRIVATE Qt6::Core)

include(GNUInstallDirs)
install(TARGETS SolTrace
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
