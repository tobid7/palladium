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

add_library(pd-desktop STATIC ${SRC})
target_include_directories(pd-desktop
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(pd-desktop PUBLIC palladium glad glfw)

target_compile_definitions(pd-desktop
    PRIVATE PD_OPENGL=${PD_GL_VERSION}
)

add_library(palladium::pd-desktop ALIAS pd-desktop)

install(TARGETS pd-desktop
    EXPORT palladiumTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(TARGETS glad
    EXPORT palladiumTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(PD_BUNDLE_GLFW)
    install(TARGETS glfw
        EXPORT palladiumTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()