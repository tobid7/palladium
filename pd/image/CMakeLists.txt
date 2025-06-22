cmake_minimum_required(VERSION 3.22)

project(pd-image LANGUAGES CXX VERSION 0.5.0)

set(SRC
    source/image.cpp
    source/img_blur.cpp
    source/img_convert.cpp
)

if(PD_BUILD_SHARED)
    pd_add_lib(pd-image BUILD_SHARED TRUE SRC_FILES ${SRC})
else()
    pd_add_lib(pd-image SRC_FILES ${SRC})
endif()
