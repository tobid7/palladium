cmake_minimum_required(VERSION 3.18)

# Setup Toolchain if not specified
# Could propably avoided by using arm-none-eabi-cmake
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(DEFINED ENV{DEVKITPRO})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{DEVKITPRO}/cmake/3DS.cmake" CACHE PATH "toolchain file")
    else()
        message(FATAL_ERROR "Please define DEVKITPRO to point to your SDK path!")
    endif()
endif()

execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_SHORT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Set Project
project(palladium LANGUAGES C CXX VERSION 0.1.8)

# Enable Compile Command Export
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Force C++ 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Set Special C and CXX flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-psabi -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS} -fno-rtti -fno-exceptions")

set(SRC_FILES
    # Core (common)
    source/common/app.cpp
    source/common/common.cpp
    source/common/strings.cpp
    source/common/timetrace.cpp
    source/common/sys.cpp
    source/common/lang.cpp
    source/common/error.cpp
    source/common/io.cpp
    # Controls
    source/controls/hid.cpp
    # Maths
    source/maths/color.cpp
    source/maths/bit_util.cpp
    source/maths/img_convert.cpp
    # Graphics
    source/graphics/texture.cpp
    source/graphics/spritesheet.cpp
    source/graphics/li7_shader.cpp
    source/graphics/lithium.cpp
    # Overlays
    source/overlays/message_mgr.cpp
    source/overlays/overlay_mgr.cpp
    source/overlays/keyboard.cpp
    source/overlays/performance.cpp
    # Tools
    source/tools/gamepad_icons.cpp
    # UI7
    source/ui7/drawlist.cpp
    source/ui7/menu.cpp
    source/ui7/theme.cpp
    source/ui7/ui7.cpp
    source/ui7/container/container.cpp
    source/ui7/container/button.cpp
    source/ui7/container/checkbox.cpp
    source/ui7/container/image.cpp
    source/ui7/container/label.cpp
    # External
    source/external/stb.cpp
)

set(TARGET_NAME palladium)

# Set Executable and its sources
add_library(${TARGET_NAME} STATIC ${SRC_FILES})

# Set dependencies, include dirs and definitions
target_include_directories(${TARGET_NAME} PUBLIC
    include
    ${DEVKITPRO}/portlibs/3ds/include
)
target_compile_definitions(${TARGET_NAME} PUBLIC
    -D_GNU_SOURCE=1
    -DPALLADIUM_VERSION="${PROJECT_VERSION}"
    -DPALLADIUM_GIT_COMMIT="${GIT_SHORT_HASH}"
    -DBUILD_CTR=1
)

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
install(TARGETS ${TARGET_NAME})
install(DIRECTORY include DESTINATION ".")