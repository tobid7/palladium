cmake_minimum_required(VERSION 3.22)

project(test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Nintendo3DS")
    if(${PD_BUILD_SHARED})
        message(ERROR "3DS Only supports Static libraries")
    endif()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-psabi -O2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS} -fno-rtti -fno-exceptions")
endif()

add_subdirectory(../ palladium)
if(${CMAKE_SYSTEM_NAME} STREQUAL "Nintendo3DS")
    add_subdirectory(../backends/3ds pd-3ds)
else()
    add_subdirectory(../backends/desktop pd-desktop)
endif()

add_executable(test source/main.cpp
    ${BKND_SRC})
if(${CMAKE_SYSTEM_NAME} STREQUAL "Nintendo3DS")
    target_link_libraries(test PUBLIC pd-3ds palladium)
else()
    target_link_libraries(test PUBLIC pd-desktop palladium)
endif()

#set(RC_FILES
#testassets/ComicNeue.ttf
#testassets/logo.png
#testassets/test.png)
#install(FILES ${RC_FILES} DESTINATION ".")
#else()
#target_link_libraries(test PUBLIC pd-backend-3ds ctru citro3d)
#endif()