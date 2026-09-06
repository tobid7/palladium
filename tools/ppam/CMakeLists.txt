cmake_minimum_required(VERSION 3.22)

project(ppam LANGUAGES CXX VERSION 1.0.0)

### Requires C++ 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_executable(ppam
    source/main.cpp
)
