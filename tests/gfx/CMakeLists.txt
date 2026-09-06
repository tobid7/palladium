cmake_minimum_required(VERSION 3.22)

project(gfx-tests)

add_executable(gfx-tests ${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp)
target_link_libraries(gfx-tests PRIVATE palladium::palladium pd-system glfw stb)
