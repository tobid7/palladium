cmake_minimum_required(VERSION 3.22)

project(pd-3ds LANGUAGES CXX VERSION 0.6.0)

set(SRC
    source/bknd-gfx.cpp
    source/bknd-hid.cpp
    source/pd-3ds.cpp
)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libpicasso)
pd_add_lib(pd-3ds SRC_FILES ${SRC})
target_include_directories(pd-3ds PUBLIC include)
target_link_libraries(pd-3ds PUBLIC m palladium ctru citro3d pica::pica)
