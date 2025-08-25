Project(SimulationRunnerUnitTests)

# Add native runner tests (always built)
add_subdirectory(native_runner)

# Add optix runner tests only if OptiX is enabled
if(SOLTRACE_BUILD_OPTIX_SUPPORT)
    add_subdirectory(optix_runner)
endif()
