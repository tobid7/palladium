cmake_minimum_required(VERSION 3.22)

## The Core Core Library
project(pd-drivers LANGUAGES CXX VERSION 0.5.0)

set(SRC
    source/hid.cpp
    source/os.cpp
    source/gfx.cpp
)

# Only Static Supported
pd_add_lib(pd-drivers SRC_FILES ${SRC})
