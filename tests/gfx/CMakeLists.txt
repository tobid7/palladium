cmake_minimum_required(VERSION 3.22)

project(gfx-tests)

add_executable(gfx-tests
	${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/os/horizon-nx.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/os/horizon-ctr.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/os/desktopos.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/os/metal-layer.cpp
)

if(APPLE AND PD_ENABLE_METAL)
	set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/source/os/metal-layer.cpp
		PROPERTIES
		LANGUAGE OBJCXX
		COMPILE_OPTIONS "-fobjc-arc"
	)
	target_link_libraries(gfx-tests PRIVATE
		"-framework Metal"
		"-framework QuartzCore"
		"-framework AppKit"
	)
endif()
target_link_libraries(gfx-tests PRIVATE palladium::palladium pd-system stb)
target_include_directories(gfx-tests
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Nintendo3DS")
	target_link_libraries(gfx-tests PRIVATE ctru)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "NintendoSwitch")
	target_link_libraries(gfx-tests
		PRIVATE
			glad
			glfw3
			EGL
			glapi
			drm_nouveau
			nx
	)
	target_compile_definitions(gfx-tests PRIVATE __SWITCH__ ENABLE_NXLINK)
else()
	target_link_libraries(gfx-tests PRIVATE glfw)
endif()
install(
	TARGETS gfx-tests
)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Nintendo3DS")
	make_3ds_target(
		"gfx-tests"
		"Palladium Gfx Tests"
		"tobid7"
		"1.0.0"
		"${CMAKE_CURRENT_SOURCE_DIR}/../assets/icon48.png"
		"${CMAKE_CURRENT_SOURCE_DIR}/../assets"
	)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "NintendoSwitch")
	make_nx_target(
		"gfx-tests"
		"Palladium Gfx Tests"
		"tobid7"
		"1.0.0"
		"${CMAKE_CURRENT_SOURCE_DIR}/../assets/icon.jpg"
		"${CMAKE_CURRENT_SOURCE_DIR}/../assets"
	)
endif()