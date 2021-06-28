option(BUILD_EXAMPLE_BOX "Build example_box.cpp" ${BUILD_ALL_EXAMPLES})
option(BUILD_EXAMPLE_CALLBACKS "Build example_callbacks.cpp" ${BUILD_ALL_EXAMPLES})
option(BUILD_EXAMPLE_CUSTOM_WIDGET_DRAWING "Build example_custom_widget_drawing.cpp" ${BUILD_ALL_EXAMPLES})
option(BUILD_EXAMPLE_CUSTOM_WIDGET_STYLING "Build example_custom_widget_styling.cpp" ${BUILD_ALL_EXAMPLES})
option(BUILD_EXAMPLE_IMAGE "Build example_image.cpp" ${BUILD_ALL_EXAMPLES})
option(BUILD_EXAMPLE_KEYBINDINGS "Build example_keybindings.cpp" ${BUILD_ALL_EXAMPLES})
option(BUILD_EXAMPLE_LABEL "Build example_label.cpp" ${BUILD_ALL_EXAMPLES})
option(BUILD_EXAMPLE_LINE_EDIT "Build example_line_edit.cpp" ${BUILD_ALL_EXAMPLES})
option(BUILD_EXAMPLE_NOTE_BOOK "Build example_note_book.cpp" ${BUILD_ALL_EXAMPLES})
option(BUILD_EXAMPLE_SPLITTER "Build example_splitter.cpp" ${BUILD_ALL_EXAMPLES})
option(BUILD_EXAMPLE_TOOLTIPS "Build example_tooltips.cpp" ${BUILD_ALL_EXAMPLES})
option(BUILD_EXAMPLE_TREE_VIEW "Build example_tree_view.cpp" ${BUILD_ALL_EXAMPLES})
option(BUILD_EXAMPLE_WIDGET_GALLERY "Build example_widget_gallery.cpp" ${BUILD_ALL_EXAMPLES})

set(examples "")
if(BUILD_EXAMPLE_BOX)
	list(APPEND examples "box.cpp")
endif()
if(BUILD_EXAMPLE_CALLBACKS)
	list(APPEND examples "callbacks.cpp")
endif()
if(BUILD_EXAMPLE_CUSTOM_WIDGET_DRAWING)
	list(APPEND examples "custom_widget_drawing.cpp")
endif()
if(BUILD_EXAMPLE_CUSTOM_WIDGET_STYLING)
	list(APPEND examples "custom_widget_styling.cpp")
endif()
if(BUILD_EXAMPLE_IMAGE)
	list(APPEND examples "image.cpp")
endif()
if(BUILD_EXAMPLE_KEYBINDINGS)
	list(APPEND examples "keybindings.cpp")
endif()
if(BUILD_EXAMPLE_LABEL)
	list(APPEND examples "label.cpp")
endif()
if(BUILD_EXAMPLE_LINE_EDIT)
	list(APPEND examples "line_edit.cpp")
endif()
if(BUILD_EXAMPLE_NOTE_BOOK)
	list(APPEND examples "note_book.cpp")
endif()
if(BUILD_EXAMPLE_SPLITTER)
	list(APPEND examples "splitter.cpp")
endif()
if(BUILD_EXAMPLE_TOOLTIPS)
	list(APPEND examples "tooltips.cpp")
endif()
if(BUILD_EXAMPLE_TREE_VIEW)
	list(APPEND examples "tree_view.cpp")
endif()
if(BUILD_EXAMPLE_WIDGET_GALLERY)
	list(APPEND examples "widget_gallery.cpp")
endif()

foreach(example ${examples})
	get_filename_component(example_name ${example} NAME_WE)
	add_executable("example_${example_name}" ${example} resources.cpp)
	set_target_properties("example_${example_name}" PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/bin"
		RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/bin"
		RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/bin"
	)
	target_include_directories("example_${example_name}" PUBLIC
		include
	)
	if (UNIX)
		target_link_libraries("example_${example_name}"
			${PROJECT_NAME} freetype SDL2main SDL2 GL X11 pthread Xrandr Xi dl
		)
	elseif (MSVC)
		target_link_libraries("example_${example_name}"
			${PROJECT_NAME}
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
