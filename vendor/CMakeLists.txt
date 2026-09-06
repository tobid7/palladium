cmake_minimum_required(VERSION 3.22)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Nintendo3DS")
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libpicasso)
else()
	# GLAD
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glad)
endif()

# STB
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/stb)

if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Nintendo3DS")
	# GLFW
	set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
	set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
	set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glfw)
endif()