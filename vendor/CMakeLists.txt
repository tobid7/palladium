cmake_minimum_required(VERSION 3.22)

# GLAD
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glad)

# STB
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/stb)