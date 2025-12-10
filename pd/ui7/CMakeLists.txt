cmake_minimum_required(VERSION 3.22)

project(pd-ui7 LANGUAGES CXX VERSION 0.6.0)

set(SRC
    source/theme.cpp
    source/ui7.cpp
    source/io.cpp
    source/layout.cpp
    source/menu.cpp

    source/container/button.cpp
    source/container/checkbox.cpp
    source/container/coloredit.cpp
    source/container/container.cpp
    source/container/dragdata.cpp
    source/container/dynobj.cpp
    source/container/image.cpp
    source/container/label.cpp
)

if(PD_BUILD_SHARED)
    pd_add_lib(pd-ui7 BUILD_SHARED TRUE SRC_FILES ${SRC})
else()
    pd_add_lib(pd-ui7 SRC_FILES ${SRC})
endif()

target_link_libraries(pd-ui7 PUBLIC pd-lithium pd-core)