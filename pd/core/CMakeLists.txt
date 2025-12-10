cmake_minimum_required(VERSION 3.22)

project(pd-core LANGUAGES CXX VERSION 0.6.0)

set(SRC
    source/bit_util.cpp
    source/color.cpp
    source/io.cpp
    source/mat.cpp
    source/strings.cpp
    source/timer.cpp
    source/timetrace.cpp
)

if(PD_BUILD_SHARED)
    pd_add_lib(pd-core BUILD_SHARED TRUE SRC_FILES ${SRC})
else()
    pd_add_lib(pd-core SRC_FILES ${SRC})
endif()

target_link_libraries(pd-core PUBLIC pd-drivers)
