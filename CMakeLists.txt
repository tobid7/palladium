cmake_minimum_required(VERSION 3.22)

include(GNUInstallDirs)

# Set Project
project(palladium LANGUAGES C CXX VERSION 0.7.0)

# Required to add this Variable
set(PD_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
include(cmake/palladium.cmake)

option(PD_BUILD_TESTS "Sets if TestApp and TestBench get build" OFF)
option(PD_BUILD_SHARED "Build Shared Library" OFF)
option(PD_BUILD_TOOLS "Build Palladium Tools" OFF)

if(${PD_BUILD_TOOLS})
    add_subdirectory(tools)
endif()

add_subdirectory(vendor)

# # Include Library Source
set(PD_SOURCES
    # Common
    source/common.cpp

    # Core
    source/core/bits.cpp
    source/core/color.cpp
    source/core/mat.cpp
    source/core/strings.cpp

    # Drivers
    source/drivers/os.cpp
    source/drivers/gfx.cpp

    # Lithium
    source/lithium/pools.cpp
)

if(${PD_BUILD_SHARED})
    add_library(palladium SHARED ${PD_SOURCES})
    target_compile_definitions(palladium PRIVATE PD_BUILD_SHARED)
else()
    add_library(palladium STATIC ${PD_SOURCES})
    target_compile_definitions(palladium PUBLIC PD_BUILD_STATIC)
endif()

target_compile_definitions(palladium PUBLIC PD_DEBUG)
target_compile_options(palladium
    PUBLIC
    $<$<CXX_COMPILER_ID:GNU,Clang>:
    -fmacro-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}/source=source
    -fmacro-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}/include=include
    >
)

add_library(palladium::palladium ALIAS palladium)

target_include_directories(palladium
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(palladium
    PRIVATE
    $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GNU,Clang>>:-O0 -g>
    $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:GNU,Clang>>:-O3>

    $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:MSVC>>:/Od /Zi>
    $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:MSVC>>:/O2>
)

install(
    TARGETS palladium
    EXPORT palladiumTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(
    DIRECTORY include/
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
    INSTALL_DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/palladium
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/palladiumConfigVersion.cmake
    VERSION
    ${PROJECT_VERSION}
    COMPATIBILITY
    SameMajorVersion
)

install(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/palladiumConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/palladiumConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/palladium
)

find_program(CLANG_FORMAT clang-format)

file(GLOB_RECURSE PD_FMTFILES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)

add_custom_target(pd-clang-format
    COMMAND ${CLANG_FORMAT} --style=file -i ${PD_FMTFILES}
    COMMENT "Formatting Project Sources"
)

add_subdirectory(backends)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
