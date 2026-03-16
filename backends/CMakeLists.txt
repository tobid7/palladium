cmake_minimum_required(VERSION 3.22)

project(pd-system)

option(PD_ENABLE_OPENGL2 "Enable OpenGL 2.1 (On Supported Hardware)" ON)
option(PD_ENABLE_OPENGL3 "Enable OpenGL 3.3 (On Supported Hardware)" ON)
option(PD_ENABLE_VULKAN "Not implemented yet" OFF)

add_library(pd-system STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/source/gfx_opengl.cpp
)
target_include_directories(pd-system
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(pd-system
    PUBLIC
    palladium::palladium
    glad
)