cmake_minimum_required(VERSION 3.22)

set(APP_TITLE "App")
set(APP_AUTHOR "tobid7")
set(APP_VERSION "${CMAKE_PROJECT_VERSION}")
set(APP_ICON "icon.png")
set(APP_ROMFS "romfs")

function(make_3ds_target _NAME TITLE AUTHOR _VERSION ICON ROMFS)
	if(${CMAKE_SYSTEM_NAME} STREQUAL "Nintendo3DS")
		find_program(3DSXTOOL 3dsxtool)
		find_program(SMDHTOOL smdhtool)
		add_custom_target(${_NAME}.smdh
			COMMAND ${SMDHTOOL} --create "${TITLE}" "${_VERSION}" "${AUTHOR}" "${ICON}"
				${_NAME}.smdh
			DEPENDS ${_NAME}.elf
			COMMENT "Creating ${_NAME}.smdh"
		)
		add_custom_target(${_NAME}.3dsx ALL
			COMMAND ${3DSXTOOL} ${_NAME}.elf ${_NAME}.3dsx --smdh=${_NAME}.smdh
				--romfs="${ROMFS}"
			DEPENDS ${_NAME}.smdh
			COMMENT "Creating ${_NAME}.3dsx"
		)
	endif()
endfunction()

function(make_nx_target _NAME TITLE AUTHOR _VERSION ICON ROMFS)
	if(${CMAKE_SYSTEM_NAME} STREQUAL "NintendoSwitch")
		find_program(ELF2NRO elf2nro)
		find_program(NACPTOOL nacptool)
		add_custom_target(${_NAME}.nacp
			COMMAND ${NACPTOOL} --create "${TITLE}" "${AUTHOR}" "${_VERSION}"
				${_NAME}.nacp
			DEPENDS ${_NAME}.elf
			COMMENT "Creating ${_NAME}.nacp"
		)
		add_custom_target(${_NAME}.nro ALL
			COMMAND ${ELF2NRO} ${_NAME}.elf ${_NAME}.nro --nacp=${_NAME}.nacp
				--romfsdir="${ROMFS}" --icon="${ICON}"
			DEPENDS ${_NAME}.nacp
			COMMENT "Creating ${_NAME}.nro"
		)
	endif()
endfunction()