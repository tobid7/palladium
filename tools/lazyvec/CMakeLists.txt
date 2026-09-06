cmake_minimum_required(VERSION 3.22)

project(lazyvec LANGUAGES CXX VERSION 2.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_executable(lazyvec
    source/main.cpp

    source/lazyconstructors.cpp
    source/lazyfuncs.cpp
    source/lazyops.cpp
    source/lazyoperation.cpp
    source/lazytemplate.cpp
    source/lazyswap.cpp
)
target_include_directories(lazyvec PUBLIC include)
