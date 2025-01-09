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

# Set Project
project(palladium LANGUAGES C CXX VERSION 1.0.0)

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
    source/common/strings.cpp
    source/common/timetrace.cpp
    source/common/sys.cpp
    source/common/lang.cpp
    source/common/error.cpp
    # Maths
    source/maths/color.cpp
    source/maths/bit_util.cpp
    source/maths/img_convert.cpp
    # Graphics
    source/graphics/texture.cpp
    source/graphics/li7_shader.cpp
    source/graphics/lithium.cpp
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
    -DVERSION="${PROJECT_VERSION}"
    -DBUILD_CTR=1
)

add_executable(test test/main.cpp)
target_include_directories(test PUBLIC include)
target_link_directories(test PUBLIC ${CMAKE_BINARY_DIR})
target_link_libraries(test PUBLIC palladium citro3d ctru m)
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
install(TARGETS ${TARGET_NAME})
install(DIRECTORY include DESTINATION ".")