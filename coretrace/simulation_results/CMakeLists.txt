
Project(simresult)

include_directories(
    . 
    ../simulation_data
)
# include_directories(cst_templates)

set(SIMRES_SRC
    bin_backend.cpp
    binner.cpp
    element_statistics.cpp
    records.cpp
    simulation_result.cpp
)

set(SIMRES_HDRS
    bin_backend.hpp
    binner.hpp
    element_statistics.hpp
    records.hpp
    simulation_result.hpp
    simulation_result_api.hpp
    simulation_result_export.hpp
)

set(POSITION_INDEPENDENT_CODE ON)

if(WIN32)
    # Hack to build on Windows
    add_library(simresult STATIC ${SIMRES_SRC})
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
