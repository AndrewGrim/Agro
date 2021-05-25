# Build all examples
file(GLOB examples "${PROJECT_SOURCE_DIR}/examples/*.cpp")
foreach(example ${examples})
	get_filename_component(example_name ${example} NAME_WE)
	add_executable("example_${example_name}" ${example})
	set_target_properties("example_${example_name}" PROPERTIES 
		RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/bin"
	)
	target_include_directories("example_${example_name}" PUBLIC
		include
	)
	if (UNIX)
		target_link_libraries("example_${example_name}"
			Agro freetype SDL2main SDL2 GL X11 pthread Xrandr Xi dl
		)
	elseif (MSVC)
		target_link_libraries("example_${example_name}"
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