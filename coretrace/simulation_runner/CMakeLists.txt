add_subdirectory(native_runner)
option(USE_OPTIX_SUPPORT "use optxi backend" ON)


# find cuda and optix libraries, if found then add optix_runner
if (USE_OPTIX_SUPPORT)

    # list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/SolTrace/cmake/")
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

    find_package(CUDAToolkit)
    find_package(OptiX)
    if(NOT CUDAToolkit_FOUND)
        message(FATAL_ERROR "CUDAToolkit not found. Please install CUDA.")
    endif()
    if(NOT OptiX_FOUND)
        message(FATAL_ERROR "OptiX not found. Please install OptiX.")
    endif()

    if(CUDAToolkit_FOUND AND OptiX_FOUND)
        message(STATUS "OptiX and CUDA found, adding optix_runner")
        # print out values of cuda_toolkit_root_dir and optix_root 
        message(STATUS "OptiX_ROOT_DIR: ${OptiX_ROOT_DIR}")

        message(STATUS "  CUDA compiler:           ${CMAKE_CUDA_COMPILER}")
        message(STATUS "  CUDA toolkit version:    ${CUDAToolkit_VERSION}")
        message(STATUS "  CUDA toolkit root dir:   ${CUDAToolkit_LIBRARY_ROOT}")


        add_subdirectory(optix_runner)
    else()
        message(STATUS "OptiX or CUDA not found, skipping optix_runner")
    endif()



endif()