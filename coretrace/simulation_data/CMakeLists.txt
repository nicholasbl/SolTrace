
Project(simdata)

include_directories(.)

set(SIMDATA_SRC
    aperture.cpp
    composite_element.cpp
    element.cpp
    matvec.cpp
    parabolic_trough.cpp
    simulation_data.cpp
    single_element.cpp
    virtual_element.cpp
    vector3d.cpp
)

set(POSITION_INDEPENDENT_CODE ON)

if(WIN32)
    # Hack to build on Windows
    add_library(simdata STATIC ${SIMDATA_SRC})
else(WIN32)
    add_library(simdata SHARED ${SIMDATA_SRC})
endif()
set_target_properties(simdata
    PROPERTIES
    DEBUG_POSTFIX "d"
	PREFIX ""
)

if (NOT APPLE AND ENABLE_COVERAGE)
	# SET(CMAKE_CXX_FLAGS "-O0 -coverage -fkeep-inline-functions")
    SET(CMAKE_CXX_FLAGS "-O0 -coverage")
endif()
