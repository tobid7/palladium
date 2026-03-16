cmake_minimum_required(VERSION 3.22)

project(core-tests)

add_executable(core-tests ${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp)
target_link_libraries(core-tests PRIVATE palladium::palladium)
