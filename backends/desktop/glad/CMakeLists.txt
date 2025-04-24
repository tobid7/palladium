cmake_minimum_required(VERSION 3.22)

add_library(glad source/glad.c)
target_include_directories(glad PUBLIC include)