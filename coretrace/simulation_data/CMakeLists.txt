
Project(simdata)

include_directories(.)

set(SIMDATA_SRC
    element.cpp
    matvec.cpp
    plane_reflector.cpp
    simulation_data.cpp
    vector3d.cpp
)

set(POSITION_INDEPENDENT_CODE ON)

add_library(simdata SHARED ${SIMDATA_SRC})
set_target_properties(simdata
    PROPERTIES
    DEBUG_POSTFIX "d"
	PREFIX ""
)
