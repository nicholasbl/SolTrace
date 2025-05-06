
Project(simdata)

include_directories(.)

set(SIMDATA_SRC
    composite_element.cpp
    element.cpp
    matvec.cpp
    simulation_data.cpp
    single_element.cpp
    virtual_element.cpp
    vector3d.cpp
)

set(POSITION_INDEPENDENT_CODE ON)

add_library(simdata STATIC ${SIMDATA_SRC})
set_target_properties(simdata
    PROPERTIES
    DEBUG_POSTFIX "d"
	PREFIX ""
)
