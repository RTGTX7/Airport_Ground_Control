
if(ESP_PLATFORM)
    idf_component_register( SRCS "main.cpp"
                            REQUIRES driver
                            INCLUDE_DIRS "." "include" $ENV{CADMIUM})

    target_compile_options(${COMPONENT_LIB} PUBLIC -std=gnu++2b)
    target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
    target_compile_options(${COMPONENT_LIB} PRIVATE "-frtti")
    target_compile_options(${COMPONENT_LIB} PRIVATE "-fexceptions")
    # target_compile_options(${COMPONENT_LIB} PRIVATE "-DNO_LOGGING")
    # target_compile_options(${COMPONENT_LIB} PRIVATE "-DNO_LOG_STATE")
    # target_compile_options(${COMPONENT_LIB} PRIVATE "-DDEBUG_DELAY")
else()

    # Regular CMake project setup for non-ESP32
    add_executable(${projectName} main.cpp)

    # Add required libraries and include directories
    target_include_directories(${projectName} PRIVATE "." "include" $ENV{CADMIUM})

    # Non-ESP32 specific compile options
    target_compile_options(${projectName} PUBLIC -std=gnu++2b)
endif()