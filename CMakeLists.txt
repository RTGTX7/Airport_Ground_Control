cmake_minimum_required(VERSION 3.16)

set(projectName "Cadmium Manual Examples")

message(STATUS "Building without embedded")
message(STATUS "Building for simulation time")
project(Airport_Ground_Control_System)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
add_subdirectory(main)

# **3. 定义源文件**
set(SOURCES
    src/Airport_Control.cpp
    src/Conflict_Avoidance.cpp
    src/Takeoff_Control.cpp
    src/Takeoff_Queue.cpp
    src/Landing_Control.cpp
    src/Landing_Queue.cpp
    src/Taxiway_Control.cpp
)

# **5. 链接 Cadmium 依赖**
target_link_libraries(airport_simulation cadmium)

# **6. 启用编译警告**
target_compile_options(airport_simulation PRIVATE -Wall -Wextra -pedantic)
