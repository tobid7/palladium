cmake_minimum_required(VERSION 3.22)

project(pd-system)

option(PD_ENABLE_OPENGL2 "Enable OpenGL 2.1 (On Supported Hardware)" ON)
option(PD_ENABLE_OPENGL3 "Enable OpenGL 3.3 (On Supported Hardware)" ON)
option(PD_ENABLE_DIRECTX9 "Enable DirectX9 Support" ON)
option(PD_ENABLE_CITRO3D "Enable Citro3D Support (3DS)" OFF)
option(PD_ENABLE_VULKAN "Not implemented yet" OFF)

if(NOT WIN32) # cause we are not on windows...
	set(PD_ENABLE_DIRECTX9 OFF)
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Nintendo3DS")
	set(PD_ENABLE_OPENGL2 OFF)
	set(PD_ENABLE_OPENGL3 OFF)
	set(PD_ENABLE_VULKAN OFF)
	set(PD_ENABLE_CITRO3D ON)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "NintendoSwitch")
	set(PD_ENABLE_OPENGL2 OFF)
	set(PD_ENABLE_OPENGL3 ON)
	set(PD_ENABLE_VULKAN OFF)
	set(PD_ENABLE_CITRO3D OFF)
endif()

add_library(pd-system STATIC
	${CMAKE_CURRENT_SOURCE_DIR}/source/gl-helper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/gfx_opengl2.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/gfx_opengl3.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/gfx_directx9.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/gfx_citro3d.cpp
)

target_include_directories(pd-system
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
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
)

# Palladium
target_link_libraries(pd-system PUBLIC palladium::palladium)

# glad (if we have any OpenGL version included)
if(PD_ENABLE_OPENGL2 OR PD_ENABLE_OPENGL3)
	target_link_libraries(pd-system
		PUBLIC glad
	)
endif()

# DirectX9
if(PD_ENABLE_DIRECTX9)
	target_link_libraries(pd-system
		PUBLIC
			d3d9
			d3dcompiler
	)
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Nintendo3DS")
	target_link_libraries(pd-system
		PUBLIC
			pica::pica
			citro3d
			ctru
	)
else()
	if(${CMAKE_SYSTEM_NAME} STREQUAL "NintendoSwitch")
		target_include_directories(pd-system
			PUBLIC $ENV{DEVKITPRO}/portlibs/switch/include
		)
	endif()
	target_link_libraries(pd-system
		PUBLIC spirv-helper
	)
endif()
