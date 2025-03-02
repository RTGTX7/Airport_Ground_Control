# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

set(projectName "Airport_Ground_Control_System")

# Define options
option(SIM "Build for simulation" OFF)

if(ESP_PLATFORM)
    message(STATUS "Building with ESP32")
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    project(${projectName})
    
else()
    message(STATUS "Building without ESP32")
    if(SIM)
        message(STATUS "Building for simulation")
        add_definitions(-DSIM_TIME)
    endif()
    project(${projectName})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
    add_subdirectory(main)
endif()