cmake_minimum_required(VERSION 3.18)

### Helper Function to Build Librarys without have always
### These includes and definition defines
function(add_pd_lib TARGET_NAME)
set(opts "BUILD_SHARED")
set(one_val_args "")
set(multi_val_args SRC_FILES DEPENDS)
cmake_parse_arguments(ARG "${opts}" "${one_val_args}" "${multi_val_args}" ${ARGN})

string(REPLACE "-" "_" FLAG_NAME_T ${TARGET_NAME})
string(TOUPPER ${FLAG_NAME_T} FLAG_NAME_F)

if(ARG_BUILD_SHARED)
    add_library(${TARGET_NAME} SHARED ${ARG_SRC_FILES})
    target_compile_definitions(${TARGET_NAME} PUBLIC -D${FLAG_NAME_F}_BUILD_SHARED=1)
    message("Building SHARED library: ${FLAG_NAME_F}_BUILD_SHARED=1")
else()
    add_library(${TARGET_NAME} STATIC ${ARG_SRC_FILES})
    target_compile_definitions(${TARGET_NAME} PUBLIC -D${FLAG_NAME_F}_BUILD_SHARED=0)
    message("Building STATIC library: ${FLAG_NAME_F}_BUILD_SHARED=0")
endif()
target_include_directories(${TARGET_NAME} PUBLIC
    include
    ${DEVKITPRO}/portlibs/3ds/include
    backends)
target_compile_definitions(${TARGET_NAME} PUBLIC
    -D_GNU_SOURCE=1
    -DPALLADIUM_VERSION="${PROJECT_VERSION}"
    -DPALLADIUM_GIT_COMMIT="${GIT_SHORT_HASH}"
    -DPALLADIUM_GIT_BRANCH="${GIT_BRANCH}"
    -DBUILD_CTR=1)
### For the libs that depend on another
if(ARG_DEPENDS)
target_link_libraries(${TARGET_NAME} PUBLIC ${ARG_DEPENDS})
endif()
install(TARGETS ${TARGET_NAME})
endfunction()

## Get Current Git Commit Value
execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_SHORT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

## Get Current Git Branch
execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Set Project
project(palladium LANGUAGES C CXX VERSION 0.4.0)

option(PD_BUILD_TESTS "Sets if TestApp and TestBench get build" OFF)
option(PD_BUILD_SHARED "Build Shared Libraries" OFF)

message("Var: ${PD_BUILD_SHARED}")

# Enable Compile Command Export
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Force C++ 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Set Special C and CXX flags for 3ds
if(${CMAKE_SYSTEM_NAME} STREQUAL "Nintendo3DS")
if(${PD_BUILD_SHARED})
message(ERROR "3DS Only supports Static libraries")
endif()
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-psabi -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS} -fno-rtti -fno-exceptions")
endif()
#include_directories(include)

## Core Source Code
set(CORE_SRC
source/core/common.cpp
source/core/bit_util.cpp
source/core/color.cpp
source/core/io.cpp
source/core/hid_driver.cpp
source/core/mat.cpp
source/core/strings.cpp
source/core/sys.cpp
source/core/timer.cpp
source/core/timetrace.cpp)

## Image Source Code
set(IMAGE_SRC
source/image/image.cpp
source/image/img_blur.cpp
source/image/img_convert.cpp)

## External Source Code
set(EXTERNAL_SRC
source/external/stb.cpp)

## Lithiu, Source Code
set(LI_SRC
source/lithium/font.cpp
source/lithium/drawlist.cpp
source/lithium/renderer.cpp)

## Net Source Code
set(NET_SRC
source/net/socket.cpp)

## UI7 Source Code
set(UI7_SRC
source/ui7/drawlist.cpp
source/ui7/io.cpp
source/ui7/layout.cpp
source/ui7/menu.cpp
source/ui7/remenu.cpp
source/ui7/theme.cpp
source/ui7/ui7.cpp
source/ui7/container/container.cpp
source/ui7/container/button.cpp
source/ui7/container/checkbox.cpp
source/ui7/container/coloredit.cpp
source/ui7/container/dragdata.cpp
source/ui7/container/dynobj.cpp
source/ui7/container/image.cpp
source/ui7/container/label.cpp)

#### Creating the Libraries ####
if(PD_BUILD_SHARED)
add_pd_lib(pd-core BUILD_SHARED TRUE SRC_FILES ${CORE_SRC})
add_pd_lib(pd-image BUILD_SHARED TRUE SRC_FILES ${IMAGE_SRC} DEPENDS pd-core)
add_pd_lib(pd-external SRC_FILES ${EXTERNAL_SRC})
add_pd_lib(pd-lithium BUILD_SHARED TRUE SRC_FILES ${LI_SRC} DEPENDS pd-core)
add_pd_lib(pd-net BUILD_SHARED TRUE SRC_FILES ${NET_SRC} DEPENDS pd-core)
add_pd_lib(pd-ui7 BUILD_SHARED TRUE SRC_FILES ${UI7_SRC} DEPENDS pd-core pd-lithium)
else()
add_pd_lib(pd-core SRC_FILES ${CORE_SRC})
add_pd_lib(pd-image SRC_FILES ${IMAGE_SRC} DEPENDS pd-core)
add_pd_lib(pd-external SRC_FILES ${EXTERNAL_SRC})
add_pd_lib(pd-lithium SRC_FILES ${LI_SRC} DEPENDS pd-core)
add_pd_lib(pd-net SRC_FILES ${NET_SRC} DEPENDS pd-core)
add_pd_lib(pd-ui7 SRC_FILES ${UI7_SRC} DEPENDS pd-core pd-lithium)
endif()

add_library(palladium INTERFACE)
target_link_libraries(palladium INTERFACE
pd-core pd-image pd-external pd-lithium pd-net pd-ui7
)

add_dependencies(palladium
pd-core pd-image pd-external pd-lithium pd-net pd-ui7
)

install(DIRECTORY include DESTINATION ".")