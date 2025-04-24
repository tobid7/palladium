cmake_minimum_required(VERSION 3.22)

if(WIN32)
set(EXTRA_LIBS ws2_32)
else()
set(EXTRA_LIBS)
endif()

add_subdirectory(glfw)
add_subdirectory(glad)

# if(PD_BUILD_SHARED)
# add_library(pd-backend-desktop SHARED
# source/li_backend_gl2.cpp
# source/pd_hid_glfw.cpp)
# target_compile_definitions(pd-backend-desktop PUBLIC
#     -DPD_BKND_DESKTOP_BUILD_SHARED=1)
# else()
add_library(pd-backend-desktop STATIC
source/li_backend_gl2.cpp
source/pd_hid_glfw.cpp)
#endif()

target_compile_definitions(pd-backend-desktop PUBLIC
     -DPD_BKND_DESKTOP_STATIC=1)
target_include_directories(pd-backend-desktop PUBLIC include)
target_link_libraries(pd-backend-desktop PUBLIC glad glfw palladium ${EXTRA_LIBS})
