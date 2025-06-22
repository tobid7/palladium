cmake_minimum_required(VERSION 3.22)

## Utilitys of Palladium Library

### RULES ###

# Force C++ 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Enable Compile Command Export
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(PD_INCLUDE_DIR "")

### Helper Function to Build Librarys without have always
### These includes and definition defines
function(pd_add_lib TARGET_NAME)
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
        ${PD_INCLUDE_DIR}
        ${DEVKITPRO}/portlibs/3ds/include
    )
    target_compile_definitions(${TARGET_NAME} PUBLIC
        -D_GNU_SOURCE=1
        -DPALLADIUM_VERSION="${PROJECT_VERSION}"
        -DPALLADIUM_GIT_COMMIT="${GIT_SHORT_HASH}"
        -DPALLADIUM_GIT_BRANCH="${GIT_BRANCH}"
        -DBUILD_CTR=1
    )
    ### For the libs that depend on another
    if(ARG_DEPENDS)
        target_link_libraries(${TARGET_NAME} PUBLIC ${ARG_DEPENDS})
    endif()
    install(TARGETS ${TARGET_NAME})
endfunction()

### GIT HELPER ###

function(pd_git_get_hash ret)
    execute_process(
        COMMAND git rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE _ret
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(${ret} "${_ret}" PARENT_SCOPE)
endfunction()

function(pd_git_get_branch ret)
    execute_process(
        COMMAND git rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE _ret
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(${ret} "${_ret}" PARENT_SCOPE)
endfunction()
