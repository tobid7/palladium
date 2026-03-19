cmake_minimum_required(VERSION 3.22)

project(pdsg LANGUAGES CXX VERSION 1.0.0)

# ## Requires C++ 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_executable(pdsg
    ${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp
)
target_link_libraries(pdsg PRIVATE
    spirv-helper::spirv-helper
)

install(
    TARGETS pdsg
)