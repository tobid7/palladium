cmake_minimum_required(VERSION 3.18)

### Helper Function to Build Librarys without have always
### These includes and definition defines
function(add_pd_lib TARGET_NAME)
set(opts "")
set(one_val_args "")
set(multi_val_args SRC_FILES DEPENDS)
cmake_parse_arguments(ARG "${opts}" "${one_val_args}" "${multi_val_args}" ${ARGN})

add_library(${TARGET_NAME} STATIC ${ARG_SRC_FILES})
target_include_directories(${TARGET_NAME} PUBLIC
    include
    ${DEVKITPRO}/portlibs/3ds/include)
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

# Setup Toolchain if not specified
# Could propably avoided by using arm-none-eabi-cmake
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(DEFINED ENV{DEVKITPRO})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{DEVKITPRO}/cmake/3DS.cmake" CACHE PATH "toolchain file")
    else()
        message(FATAL_ERROR "Please define DEVKITPRO to point to your SDK path!")
    endif()
endif()

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
project(palladium LANGUAGES C CXX VERSION 0.2.5)

option(PD_BUILD_TESTS "Sets if TestApp and TestBench get build" OFF)

# Enable Compile Command Export
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Force C++ 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Set Special C and CXX flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-psabi -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS} -fno-rtti -fno-exceptions")

## Core Source Code
set(CORE_SRC
source/core/common.cpp
source/core/bit_util.cpp
source/core/color.cpp
source/core/io.cpp
source/core/strings.cpp
source/core/sys.cpp
source/core/timer.cpp
source/core/timetrace.cpp)

## Image Source Code
set(IMAGE_SRC
source/image/img_blur.cpp
source/image/img_convert.cpp)

## External Source Code
set(EXTERNAL_SRC
source/external/stb.cpp)

## Drivers Source Code
set(DRVS_SRC
source/drivers/hid.cpp)

## Lib3ds Source Code
set(L3DS_SRC
source/lib3ds/gamepad_icons.cpp
source/lib3ds/result_decoder.cpp
source/lib3ds/drv_hid.cpp
source/lib3ds/hwinfo.cpp
source/lib3ds/os.cpp)

## Lithium Source Code
set(LI_SRC
source/lithium/li7_shader.cpp
source/lithium/spritesheet.cpp
source/lithium/texture.cpp
source/lithium/font.cpp
source/lithium/objects.cpp
source/lithium/renderer.cpp)

## Sounbd Source Code
set(SOUND_SRC
source/sound/mp3.cpp)

## Overlay source Code
set(OVL_SRC
source/overlays/keyboard.cpp
source/overlays/message_mgr.cpp
source/overlays/overlay_mgr.cpp
source/overlays/performance.cpp
source/overlays/settings.cpp)

## App Source Code
set(APP_SRC
source/app/app.cpp
source/app/lang.cpp
source/app/error.cpp)

## UI7 Source Code
set(UI7_SRC
source/ui7/drawlist.cpp
source/ui7/menu.cpp
source/ui7/theme.cpp
source/ui7/ui7.cpp
source/ui7/container/container.cpp
source/ui7/container/button.cpp
source/ui7/container/checkbox.cpp
source/ui7/container/image.cpp
source/ui7/container/label.cpp)

#### Creating the Libraries (if activated) ####
add_pd_lib(pd-core SRC_FILES ${CORE_SRC})
add_pd_lib(pd-image SRC_FILES ${IMAGE_SRC} DEPENDS pd-core)
add_pd_lib(pd-external SRC_FILES ${EXTERNAL_SRC})
add_pd_lib(pd-drivers SRC_FILES ${DRVS_SRC} DEPENDS pd-core)
add_pd_lib(pd-lib3ds SRC_FILES ${L3DS_SRC} DEPENDS pd-drivers)
add_pd_lib(pd-lithium SRC_FILES ${LI_SRC} DEPENDS pd-core pd-image pd-external citro3d)
add_pd_lib(pd-sound SRC_FILES ${SOUND_SRC} DEPENDS pd-core mpg123)
add_pd_lib(pd-overlays SRC_FILES ${OVL_SRC} DEPENDS pd-lithium)
add_pd_lib(pd-app SRC_FILES ${APP_SRC} DEPENDS pd-overlays pd-drivers pd-lib3ds)
add_pd_lib(pd-ui7 SRC_FILES ${UI7_SRC} DEPENDS pd-drivers pd-lithium)

add_library(palladium INTERFACE)
target_link_libraries(palladium INTERFACE
pd-core pd-image pd-external pd-drivers pd-lib3ds
pd-lithium pd-overlays pd-app pd-ui7
)

add_dependencies(palladium
pd-core pd-image pd-external pd-drivers
pd-lib3ds pd-lithium pd-overlays pd-app pd-ui7
)

add_library(palladium-lite INTERFACE)
target_link_libraries(palladium-lite INTERFACE
pd-core pd-image pd-external pd-drivers pd-lib3ds
pd-lithium
)

add_dependencies(palladium-lite
pd-core pd-image pd-external pd-drivers
pd-lib3ds pd-lithium
)

if(PD_BUILD_TESTS)
add_executable(test test/app/main.cpp)
target_include_directories(test PUBLIC include test/app)
target_link_directories(test PUBLIC ${CMAKE_BINARY_DIR})
target_link_libraries(test PUBLIC palladium citro3d ctru m)

add_executable(testbench test/bench/main.cpp)
target_include_directories(testbench PUBLIC include test/bench)
target_link_directories(testbench PUBLIC ${CMAKE_BINARY_DIR})
target_link_libraries(testbench PUBLIC palladium citro3d ctru m)
# Generate 3DSX
ctr_generate_smdh(
    ${CMAKE_BINARY_DIR}/test.smdh
    NAME "${APP_NAME}"
    DESCRIPTION "Palladium test app"
    AUTHOR "tobid7"
    ICON "test/romfs/icon.png"
)
ctr_create_3dsx(
    test
    OUTPUT "${CMAKE_BINARY_DIR}/test.3dsx"
    SMDH "${CMAKE_BINARY_DIR}/test.smdh"
    ROMFS "${CMAKE_SOURCE_DIR}/test/romfs"
)
ctr_create_3dsx(
    testbench
    OUTPUT "${CMAKE_BINARY_DIR}/testbench.3dsx"
    SMDH "${CMAKE_BINARY_DIR}/test.smdh"
    ROMFS "${CMAKE_SOURCE_DIR}/test/romfs"
)
endif()

install(DIRECTORY include DESTINATION ".")