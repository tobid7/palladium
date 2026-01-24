cmake_minimum_required(VERSION 3.22)

project(pd-desktop LANGUAGES CXX VERSION 0.5.0)

set(PD_GL_VERSION 21 CACHE STRING "OpenGL Version (2.1 -> 21, 3.3 -> 33)")

if(APPLE)
set(PD_GL_VERSION 33 CACHE STRING "33" FORCE)
endif()

message("Using OpenGL: " ${PD_GL_VERSION})

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

target_compile_definitions(pd-desktop
    PRIVATE PD_OPENGL=${PD_GL_VERSION}
)