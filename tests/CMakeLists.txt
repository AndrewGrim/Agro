# Build all tests
file(GLOB tests "${PROJECT_SOURCE_DIR}/tests/*.cpp")
foreach(test ${tests})
	get_filename_component(test_name ${test} NAME_WE)
	add_executable("test_${test_name}" ${test})
	set_target_properties("test_${test_name}" PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/bin"
		RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/bin"
		RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/bin"
	)
	target_include_directories("test_${test_name}" PUBLIC
		include
	)
	if (UNIX)
		target_link_libraries("test_${test_name}"
			Agro freetype SDL2main SDL2 GL X11 pthread Xrandr Xi dl
		)
	elseif (MSVC)
		target_link_libraries("test_${test_name}"
			Agro
			../../msvc_lib/FreeType2/freetype
			../../msvc_lib/SDL2/SDL2main
			../../msvc_lib/SDL2/SDL2
			opengl32
			winmm
			imm32
			version
			setupapi
		)
	endif()
endforeach()
