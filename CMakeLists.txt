# Chunky CMake
# ------------------------------------------------------------------------------
# 
# For the love of god keep this simple!
# 
# Just because CMake is turning complete isn't an excuse to write a ton of bat
# shit insaine abstractions

# ------------------------------------------------------------------------------
# CMake Settings

cmake_minimum_required(VERSION 3.16)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ------------------------------------------------------------------------------
# Solution

get_filename_component(DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${DIR_NAME})

# ------------------------------------------------------------------------------
# Utest

add_executable(
        utest
        include/chunky.h
        src/blocks.c
        src/chunky_ctx.h
        src/components.c
        src/entities.c
        src/lifetime.c
        test/utest.c
        test/utest_blocks.c
        test/utest_components.c
        test/utest_entities.c
        test/utest_lifetime.c
        test/utest_recorder.c)

target_include_directories(
        utest
        PRIVATE
        include
        external/utest.h)

set_target_properties(
        utest 
        PROPERTIES
        C_STANDARD 11)

# ------------------------------------------------------------------------------
# IMGUI
# Modified from extenal/cimgui-sokol-starterkit/CMakeLists.txt

# Linux -pthread shenanigans
if (CMAKE_SYSTEM_NAME STREQUAL Linux)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
endif()

add_library(cimgui STATIC 
        external/cimgui-sokol-starterkit/cimgui/cimgui.cpp 
        external/cimgui-sokol-starterkit/cimgui/cimgui.h 
        external/cimgui-sokol-starterkit/cimgui/imgui/imgui.cpp
        external/cimgui-sokol-starterkit/cimgui/imgui/imgui.h
        external/cimgui-sokol-starterkit/cimgui/imgui/imgui_widgets.cpp 
        external/cimgui-sokol-starterkit/cimgui/imgui/imgui_draw.cpp 
        external/cimgui-sokol-starterkit/cimgui/imgui/imgui_tables.cpp
        external/cimgui-sokol-starterkit/cimgui/imgui/imgui_demo.cpp)

target_include_directories(cimgui INTERFACE external/cimgui-sokol-starterkit/cimgui)

set(SOKOL_HEADERS
        external/cimgui-sokol-starterkit/sokol/sokol_gfx.h
        external/cimgui-sokol-starterkit/sokol/sokol_app.h
        external/cimgui-sokol-starterkit/sokol/sokol_time.h
        external/cimgui-sokol-starterkit/sokol/sokol_imgui.h
        external/cimgui-sokol-starterkit/sokol/sokol_glue.h)

if(CMAKE_SYSTEM_NAME STREQUAL Darwin)
        add_library(sokol STATIC external/cimgui-sokol-starterkit/sokol/sokol.m ${SOKOL_HEADERS})
        target_link_libraries(sokol PUBLIC
                "-framework QuartzCore"
                "-framework Cocoa"
                "-framework MetalKit"
                "-framework Metal")
else()
        add_library(sokol STATIC external/cimgui-sokol-starterkit/sokol/sokol.c ${SOKOL_HEADERS})
        
        if (CMAKE_SYSTEM_NAME STREQUAL Linux)
                target_link_libraries(sokol INTERFACE X11 Xi Xcursor GL dl m)
                target_link_libraries(sokol PUBLIC Threads::Threads)
        endif()
endif()

target_link_libraries(sokol PUBLIC cimgui)
target_include_directories(sokol INTERFACE external/cimgui-sokol-starterkit/sokol PUBLIC external/cimgui-sokol-starterkit/cimgui)

#=== EXECUTABLE: demo ===#

set(IMGUI_FILES
        include/chunky.h
        src/blocks.c
        src/chunky_ctx.h
        src/components.c
        src/entities.c
        src/lifetime.c)

if(CMAKE_SYSTEM_NAME STREQUAL Windows)
        add_executable(imgui WIN32 ${IMGUI_FILES} test/imgui_app.c)
else()
        add_executable(imgui ${IMGUI_FILES} test/imgui_app.c)
endif()

target_compile_definitions(imgui PRIVATE "CHUNKY_MAX_BLOCKS=8" "CHUNKY_MAX_ENTITIES=32")
target_include_directories(imgui PRIVATE include src)
target_link_libraries(imgui sokol)
