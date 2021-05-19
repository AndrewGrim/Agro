# Build all tests
file(GLOB tests "${PROJECT_SOURCE_DIR}/tests/*.cpp")
foreach(test ${tests})
	get_filename_component(test_name ${test} NAME_WE)
	add_executable("test_${test_name}" ${test})
	target_include_directories("test_${test_name}" PUBLIC
		include/glm
	)
	if (UNIX)
		target_link_libraries("test_${test_name}"
			gui freetype SDL2main SDL2 GL X11 pthread Xrandr Xi dl
		)
	elseif (MSVC)
		target_link_libraries("test_${test_name}"
			gui 
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