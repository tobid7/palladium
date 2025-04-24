cmake_minimum_required(VERSION 3.22)

# Make sure to use 3ds toolschain for 3ds build :)
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(DEFINED ENV{DEVKITPRO})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{DEVKITPRO}/cmake/3DS.cmake" CACHE PATH "toolchain file")
    else()
        message(FATAL_ERROR "Please define DEVKITPRO to point to your SDK path!")
    endif()
endif()
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-psabi -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS} -fno-rtti -fno-exceptions")

add_library(pd-backend-3ds STATIC
source/li_backend_c3d.cpp
source/pd_hid_3ds.cpp)
target_include_directories(pd-backend-3ds PUBLIC include)
target_link_libraries(pd-backend-3ds PUBLIC palladium)