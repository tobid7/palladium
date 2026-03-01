cmake_minimum_required(VERSION 3.22)

include(GNUInstallDirs)

# Set Project
project(palladium LANGUAGES C CXX VERSION 0.6.0)

# Required to add this Variable
set(PD_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
include(cmake/palladium.cmake)

option(PD_BUILD_TESTS "Sets if TestApp and TestBench get build" OFF)
option(PD_BUILD_SHARED "Build Shared Library" OFF)
option(PD_BUILD_TOOLS "Build Palladium Tools" OFF)
option(PD_BUILD_DESKTOP "Build Desktop Backend" OFF)
option(PD_BUILD_3DS "Build 3ds Backend" OFF)
option(PD_BUNDLE_GLFW "Bundle GLFW in install" OFF)

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
    source/drivers/context.cpp
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
    target_compile_definitions(palladium PRIVATE -DPD_BUILD_SHARED)
else()
    add_library(palladium STATIC ${PD_SOURCES})
    target_compile_definitions(palladium PUBLIC -DPD_BUILD_STATIC)
endif()

add_library(palladium::palladium ALIAS palladium)

target_include_directories(palladium
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Nintendo3DS")
    target_compile_options(palladium PRIVATE
        $<$<COMPILE_LANGUAGE:C>:-Wall>
        $<$<COMPILE_LANGUAGE:C>:-Wno-psabi>
        $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
    )
endif()

target_compile_options(palladium PRIVATE
    $<$<CONFIG:Debug>:-O0 -g>
    $<$<CONFIG:Release>:-O3>
)

if(PD_BUILD_DESKTOP AND NOT PD_BUILD_3DS)
    add_subdirectory(backends/desktop)
endif()
if(PD_BUILD_3DS AND NOT PD_BUILD_DESKTOP)
    add_subdirectory(backends/3ds)
endif()

install(TARGETS palladium
    EXPORT palladiumTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(EXPORT palladiumTargets
    FILE palladiumTargets.cmake
    NAMESPACE palladium::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/palladium
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    cmake/palladiumConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/palladiumConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/palladium
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/palladiumConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/palladiumConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/palladiumConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/palladium
)


find_program(CLANG_FORMAT clang-format)

file(GLOB_RECURSE PD_FMTFILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/pd/core/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/pd/drivers/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/pd/image/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/pd/lithium/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/pd/ui7/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backends/desktop/include/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backends/3ds/include/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backends/desktop/include/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backends/3ds/include/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/source*.cpp
)

add_custom_target(pd-clang-format
    COMMAND ${CLANG_FORMAT} --style=file -i ${PD_FMTFILES}
    COMMENT "Formatting Project Sources"
)
