cmake_minimum_required(VERSION 3.22)

project(test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_INSTALL_BINDIR ".")
add_subdirectory(.. palladium)
if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Nintendo3DS")
add_subdirectory(../backends/desktop backend-desktop)
else()
add_subdirectory(../backends/3ds backend-3ds)
endif()

add_executable(test source/main.cpp
${BKND_SRC})
target_include_directories(test PUBLIC ../backends)
if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Nintendo3DS")
target_link_libraries(test PUBLIC pd-backend-desktop glad glfw)
install(TARGETS test RUNTIME DESTINATION ".")
set(RC_FILES
testassets/ComicNeue.ttf
testassets/logo.png
testassets/test.png)
install(FILES ${RC_FILES} DESTINATION ".")
else()
target_link_libraries(test PUBLIC pd-backend-3ds ctru citro3d)
endif()