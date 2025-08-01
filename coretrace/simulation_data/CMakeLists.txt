
Project(simdata)

include_directories(.)
include_directories(cst_templates)

set(SIMDATA_SRC
    aperture.cpp
    arclength.cpp
    composite_element.cpp
    element.cpp
    matvec.cpp
    simulation_data.cpp
    single_element.cpp
    stage_element.cpp
    virtual_element.cpp
    vector3d.cpp
    cst_templates/heliostat.cpp
    cst_templates/linear_fresnel.cpp
    cst_templates/parabolic_dish.cpp
    cst_templates/parabolic_trough.cpp
)

# Only headers in simulation_data (including cst_templates subfolder)
set(SIMDATA_HDRS
    aperture.hpp
    arclength.hpp
    composite_element.hpp
    container.hpp
    datetime.hpp
    element.hpp
    error_distributions.hpp
    matvec.hpp
    optical_properties.hpp
    ray_source.hpp
    simulation_data.hpp
    simulation_parameters.hpp
    single_element.hpp
    stage_element.hpp
    sun.hpp
    surface.hpp
    vector3d.hpp
    virtual_element.hpp
    cst_templates/heliostat.hpp
    cst_templates/linear_fresnel.hpp
    cst_templates/parabolic_dish.hpp
    cst_templates/parabolic_trough.hpp
)

source_group("Header Files" FILES ${SIMDATA_HDRS})

set(POSITION_INDEPENDENT_CODE ON)

if(WIN32)
    # Hack to build on Windows
    add_library(simdata STATIC ${SIMDATA_SRC} ${SIMDATA_HDRS})
else(WIN32)
    add_library(simdata SHARED ${SIMDATA_SRC} ${SIMDATA_HDRS})
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
