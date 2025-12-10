cmake_minimum_required(VERSION 3.22)

# Set Project
project(palladium LANGUAGES C CXX VERSION 0.6.0)

# Required to add this Variable
set(PD_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
include(cmake/palladium.cmake)

option(PD_BUILD_TESTS "Sets if TestApp and TestBench get build" OFF)
option(PD_BUILD_SHARED "Build Shared Libraries" OFF)
option(PD_BUILD_TOOLS "Build Palladium Tools" OFF)

if(${PD_BUILD_TOOLS})
    add_subdirectory(tools)
endif()

## Include Library Source

add_subdirectory(pd/drivers)
add_subdirectory(pd/core)
add_subdirectory(pd/image)
add_subdirectory(pd/external)
add_subdirectory(pd/lithium)
add_subdirectory(pd/ui7)

add_library(palladium INTERFACE)
target_link_libraries(palladium INTERFACE
    pd-core pd-image pd-external pd-lithium pd-ui7 #pd-net
)

add_dependencies(palladium
    pd-drivers pd-core pd-image pd-external pd-lithium pd-ui7 #pd-net
)

install(DIRECTORY include DESTINATION ".")
