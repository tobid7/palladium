cmake_minimum_required(VERSION 3.22)

project(pd-external LANGUAGES CXX VERSION 0.5.0)

set(SRC
    source/stb.cpp
)

pd_add_lib(pd-external SRC_FILES ${SRC})
