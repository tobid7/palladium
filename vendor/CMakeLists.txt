cmake_minimum_required(VERSION 3.22)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Nintendo3DS")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libpicasso)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "NintendoSwitch")
    if(PD_BUILD_BACKENDS)
        # SPIRV-Helper
        if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
            # For some reason cmake is generating wrong headers for glslang on windows
            set(SPV_EXCLUDE_GLSLANG ON CACHE BOOL "")
        endif()
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/spirv-helper)
    endif()
else()
    if(PD_BUILD_BACKENDS)
        # GLAD
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glad)
        # SPIRV-Helper
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/spirv-helper)
    endif()
endif()

# STB
add_library(stb INTERFACE)
target_include_directories(stb
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/stb>
    $<INSTALL_INTERFACE:include>
)
install(TARGETS stb
    EXPORT palladiumTargets
    INCLUDES DESTINATION include
)

if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Nintendo3DS" AND NOT ${CMAKE_SYSTEM_NAME} STREQUAL "NintendoSwitch" AND PD_BUILD_BACKENDS)
    # GLFW
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glfw)
endif()
