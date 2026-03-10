cmake_minimum_required(VERSION 3.22)

project(pd-3ds LANGUAGES CXX VERSION 0.6.0)

set(SRC
    source/bknd-gfx.cpp
    source/bknd-hid.cpp
    source/pd-3ds.cpp
)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libpicasso)
add_library(pd-3ds STATIC ${SRC})
target_include_directories(pd-3ds PUBLIC
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(pd-3ds PUBLIC m palladium ctru citro3d pica::pica)
add_library(palladium::pd-3ds ALIAS pd-3ds)
install(TARGETS pd-3ds
    EXPORT palladiumTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(TARGETS pica
    EXPORT palladiumTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
