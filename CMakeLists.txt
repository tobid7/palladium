cmake_minimum_required(VERSION 3.22)

# Set Project
project(palladium LANGUAGES C CXX VERSION 0.6.0)

# Required to add this Variable
set(PD_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
include(cmake/palladium.cmake)

option(PD_BUILD_TESTS "Sets if TestApp and TestBench get build" OFF)
option(PD_BUILD_SHARED "Build Shared Library" OFF)
option(PD_BUILD_TOOLS "Build Palladium Tools" OFF)

if(${PD_BUILD_TOOLS})
    add_subdirectory(tools)
endif()

# # Include Library Source
set(PD_SOURCES

    # Core
    source/core/bit_util.cpp
    source/core/color.cpp
    source/core/io.cpp
    source/core/mat.cpp
    source/core/strings.cpp
    source/core/timer.cpp
    source/core/timetrace.cpp

    # Drivers
    source/drivers/gfx.cpp
    source/drivers/hid.cpp
    source/drivers/os.cpp

    # External
    source/external/stb.cpp

    # Image
    source/image/image.cpp
    source/image/img_blur.cpp
    source/image/img_convert.cpp

    # Lithium
    source/lithium/command.cpp
    source/lithium/drawlist.cpp
    source/lithium/font.cpp
    source/lithium/fonts.cpp
    source/lithium/renderer.cpp

    # UI7
    source/ui7/container/button.cpp
    source/ui7/container/checkbox.cpp
    source/ui7/container/coloredit.cpp
    source/ui7/container/container.cpp
    source/ui7/container/dragdata.cpp
    source/ui7/container/dynobj.cpp
    source/ui7/container/image.cpp
    source/ui7/container/label.cpp
    source/ui7/container/slider.cpp
    source/ui7/io.cpp
    source/ui7/layout.cpp
    source/ui7/menu.cpp
    source/ui7/theme.cpp
    source/ui7/ui7.cpp
)

if(${PD_BUILD_SHARED})
    add_library(palladium SHARED ${PD_SOURCES})
else()
    add_library(palladium STATIC ${PD_SOURCES})
endif()

target_include_directories(palladium PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(DIRECTORY include DESTINATION ".")
install(TARGETS palladium)

find_program(CLANG_FORMAT clang-format)

file(GLOB_RECURSE PD_FMTFILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/pd/core/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/pd/drivers/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/pd/image/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/pd/lithium/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/pd/ui7/*.hpp
)

message(STATUS "Files to format: ${PD_FMTFILES}")

add_custom_target(pd-clang-format
    COMMAND ${CLANG_FORMAT} --style=file -i ${PD_FMTFILES}
    COMMENT "Formatting Project Sources"
)