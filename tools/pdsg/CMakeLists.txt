cmake_minimum_required(VERSION 3.22)

project(pdsg LANGUAGES CXX VERSION 1.0.0)

### Requires C++ 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_executable(pdsg
    ${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/spirv-helper.cpp
)
target_include_directories(pdsg PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(pdsg PRIVATE
    glslang
    glslang-default-resource-limits
    OSDependent
    SPIRV

    spirv-cross-core
    spirv-cross-cpp
    spirv-cross-glsl
    spirv-cross-hlsl
    spirv-cross-msl
    spirv-cross-reflect
    spirv-cross-util
)
