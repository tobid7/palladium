cmake_minimum_required(VERSION 3.22)

project(pd-system)

option(PD_ENABLE_OPENGL2 "Enable OpenGL 2.1 (On Supported Hardware)" ON)
option(PD_ENABLE_OPENGL3 "Enable OpenGL 3.3 (On Supported Hardware)" ON)
option(PD_ENABLE_DIRECTX9 "Enable DirectX9 Support" ON)
option(PD_ENABLE_CITRO3D "Enable Citro3D Support (3DS)" OFF)
option(PD_ENABLE_VULKAN "Not implemented yet" OFF)
option(PD_ENABLE_HID_GLFW "Enable GLFW Input Driver" ON)
option(PD_ENABLE_HID_NX "Enable NX Input Driver" OFF)

if(NOT WIN32) # cause we are not on windows...
    set(PD_ENABLE_DIRECTX9 OFF)
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Nintendo3DS")
    set(PD_ENABLE_OPENGL2 OFF)
    set(PD_ENABLE_OPENGL3 OFF)
    set(PD_ENABLE_VULKAN OFF)
    set(PD_ENABLE_CITRO3D ON)
    set(PD_ENABLE_HID_GLFW OFF)
    set(PD_ENABLE_HID_NX OFF)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "NintendoSwitch")
    set(PD_ENABLE_OPENGL2 OFF)
    set(PD_ENABLE_OPENGL3 ON)
    set(PD_ENABLE_VULKAN OFF)
    set(PD_ENABLE_CITRO3D OFF)
    set(PD_ENABLE_HID_NX ON) # Best case for Nintendo Switch
    set(PD_ENABLE_HID_GLFW ON) # Technically supported but not recommended
endif()

add_library(pd-system STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/source/gl-helper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/gfx_opengl2.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/gfx_opengl3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/gfx_directx9.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/gfx_citro3d.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/hid_glfw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/hid_nx.cpp
)

target_include_directories(pd-system
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    # Why is this not a default include (same problem as with the __SWITCH__ define)
    $<$<STREQUAL:${CMAKE_SYSTEM_NAME},NintendoSwitch>:${DEVKITPRO}/portlibs/switch/include>
)

target_compile_options(palladium
    PUBLIC $<$<CXX_COMPILER_ID:GNU,Clang>:
    -fmacro-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}/source=source
    -fmacro-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}/include=include
    >
)

target_compile_definitions(pd-system
    PUBLIC
    $<$<BOOL:${PD_ENABLE_OPENGL2}>:PD_ENABLE_OPENGL2>
    $<$<BOOL:${PD_ENABLE_OPENGL3}>:PD_ENABLE_OPENGL3>
    $<$<BOOL:${PD_ENABLE_VULKAN}>:PD_ENABLE_VULKAN>
    $<$<BOOL:${PD_ENABLE_DIRECTX9}>:PD_ENABLE_DIRECTX9>
    $<$<BOOL:${PD_ENABLE_CITRO3D}>:PD_ENABLE_CITRO3D>
    $<$<BOOL:${PD_ENABLE_HID_GLFW}>:PD_ENABLE_HID_GLFW>
    $<$<BOOL:${PD_ENABLE_HID_NX}>:PD_ENABLE_HID_NX>
)

# Palladium
target_link_libraries(pd-system PUBLIC palladium::palladium)


# Depandant Lib includes (i love this cmake feature)
target_link_libraries(pd-system PUBLIC
    $<$<BOOL:${PD_ENABLE_DIRECTX9}>:d3d9 d3dcompiler> # DirectX9
    $<$<BOOL:${PD_ENABLE_CITRO3D}>:pica::pica citro3d ctru> # 3ds
    $<$<BOOL:${PD_ENABLE_OPENGL3}>:spirv-helper> # OpenGL3
    # Include Glad if we have any OpenGL Usage
    $<$<OR:$<BOOL:${PD_ENABLE_OPENGL2}>,$<BOOL:${PD_ENABLE_OPENGL3}>>:glad>
    $<$<BOOL:${PD_ENABLE_HID_NX}>:nx> # Hid NX requirement
    # Hid GLFW requirement
    $<$<BOOL:${PD_ENABLE_HID_GLFW}>:
    $<$<STREQUAL:${CMAKE_SYSTEM_NAME},NintendoSwitch>:glfw3>
    $<$<NOT:$<STREQUAL:${CMAKE_SYSTEM_NAME},NintendoSwitch>>:glfw>>

)
