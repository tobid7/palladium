cmake_minimum_required(VERSION 3.22)

project(pd-lithium LANGUAGES CXX VERSION 0.5.0)

set(SRC
    source/drawlist.cpp
    source/font.cpp
    source/renderer.cpp
)

if(PD_BUILD_SHARED)
    pd_add_lib(pd-lithium BUILD_SHARED TRUE SRC_FILES ${SRC})
else()
    pd_add_lib(pd-lithium SRC_FILES ${SRC})
endif()

target_link_libraries(pd-lithium PUBLIC pd-core)
