cmake_minimum_required(VERSION 3.22)

project(glad LANGUAGES C)

add_library(glad STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/glad.c
)
target_include_directories(glad
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)