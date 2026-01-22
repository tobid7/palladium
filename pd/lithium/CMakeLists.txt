cmake_minimum_required(VERSION 3.22)

project(pd-lithium LANGUAGES CXX VERSION 0.6.0)

option(PD_LI_INCLUDE_FONTS "Include Fonts" OFF)

set(SRC
    source/command.cpp
    source/drawlist.cpp
    source/font.cpp
    source/fonts.cpp
    source/renderer.cpp
)

if(PD_BUILD_SHARED)
    pd_add_lib(pd-lithium BUILD_SHARED TRUE SRC_FILES ${SRC})
else()
    pd_add_lib(pd-lithium SRC_FILES ${SRC})
endif()

if(${PD_LI_INCLUDE_FONTS})
    target_compile_definitions(pd-lithium PUBLIC
        -DPD_LI_INCLUDE_FONTS=1
    )
endif()

target_link_libraries(pd-lithium PUBLIC pd-core)
