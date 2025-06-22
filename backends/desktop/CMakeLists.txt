cmake_minimum_required(VERSION 3.22)

project(pd-desktop LANGUAGES CXX VERSION 0.5.0)

add_subdirectory(glad)
add_subdirectory(glfw)

set(SRC
    source/bknd-gfx.cpp
    source/bknd-hid.cpp
    source/pd-desktop.cpp
)

pd_add_lib(pd-desktop SRC_FILES ${SRC})
target_include_directories(pd-desktop PUBLIC include)
target_link_libraries(pd-desktop PUBLIC palladium glad glfw)
