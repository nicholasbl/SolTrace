
Project(simresult)

include_directories(
    . 
    ../simulation_data
)
# include_directories(cst_templates)

set(SIMRES_SRC
    simulation_results.cpp
)

set(POSITION_INDEPENDENT_CODE ON)

if(WIN32)
    # Hack to build on Windows
    add_library(simdresult STATIC ${SIMRES_SRC})
else(WIN32)
    add_library(simresult SHARED ${SIMRES_SRC})
endif()

set_target_properties(simresult
    PROPERTIES
    DEBUG_POSTFIX "d"
	PREFIX ""
)

target_link_libraries(
    simresult
    simdata
)

if (NOT APPLE AND ENABLE_COVERAGE)
	# SET(CMAKE_CXX_FLAGS "-O0 -coverage -fkeep-inline-functions")
    SET(CMAKE_CXX_FLAGS "-O0 -coverage")
endif()
