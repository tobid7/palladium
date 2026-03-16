cmake_minimum_required(VERSION 3.22)

## Utilitys of Palladium Library

### RULES ###

# Force C++ 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Enable Compile Command Export
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
