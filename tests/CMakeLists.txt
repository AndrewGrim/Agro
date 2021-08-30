option(BUILD_TEST_CLIP "Build test_clip.cpp" ${BUILD_ALL_TESTS})
option(BUILD_TEST_COLOR "Build test_color.cpp" ${BUILD_ALL_TESTS})
option(BUILD_TEST_COMPLEX_CLIPPING "Build test_complex_clipping.cpp" ${BUILD_ALL_TESTS})
option(BUILD_TEST_ONE_MILLION_BUTTONS "Build test_one_million_buttons.cpp" ${BUILD_ALL_TESTS})
option(BUILD_TEST_SCROLLED_BOX_BOTH "Build test_scrolled_box_both.cpp" ${BUILD_ALL_TESTS})
option(BUILD_TEST_SCROLLED_BOX_INCEPTION_CLIPPING "Build test_scrolled_box_inception_clipping.cpp" ${BUILD_ALL_TESTS})
option(BUILD_TEST_SCROLLED_BOX_INNER "Build test_scrolled_box_inner.cpp" ${BUILD_ALL_TESTS})
option(BUILD_TEST_SCROLLED_BOX_OUTER "Build test_scrolled_box_outer.cpp" ${BUILD_ALL_TESTS})

set(tests "")
if(BUILD_TEST_CLIP)
	list(APPEND tests "clip.cpp")
endif()
if(BUILD_TEST_COLOR)
	list(APPEND tests "color.cpp")
endif()
if(BUILD_TEST_COMPLEX_CLIPPING)
	list(APPEND tests "complex_clipping.cpp")
endif()
if(BUILD_TEST_ONE_MILLION_BUTTONS)
	list(APPEND tests "one_million_buttons.cpp")
endif()
if(BUILD_TEST_SCROLLED_BOX_BOTH)
	list(APPEND tests "scrolled_box_both.cpp")
endif()
if(BUILD_TEST_SCROLLED_BOX_INCEPTION_CLIPPING)
	list(APPEND tests "scrolled_box_inception_clipping.cpp")
endif()
if(BUILD_TEST_SCROLLED_BOX_INNER)
	list(APPEND tests "scrolled_box_inner.cpp")
endif()
if(BUILD_TEST_SCROLLED_BOX_OUTER)
	list(APPEND tests "scrolled_box_outer.cpp")
endif()

foreach(test ${tests})
	get_filename_component(test_name ${test} NAME_WE)
	add_executable("test_${test_name}" ${test})
	set_target_properties("test_${test_name}" PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/bin"
		RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/bin"
		RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/bin"
	)
	if (UNIX)
		target_link_libraries("test_${test_name}"
			${PROJECT_NAME}
			GL
			X11
			pthread
			Xrandr
			Xi
			dl
		)
	elseif (MSVC)
		target_link_libraries("test_${test_name}"
			${PROJECT_NAME}
			opengl32
			winmm
			imm32
			version
			setupapi
		)
	endif()
endforeach()
