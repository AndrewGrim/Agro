cmake_minimum_required(VERSION 3.13.4)
project(gui)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-parameter -Wpedantic")
	set(CMAKE_CXX_FLAGS_DEBUG "-g")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set(CMAKE_CXX_FLAGS "/Wall")
	set(CMAKE_CXX_FLAGS_DEBUG "/DEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE "/O2")
endif()
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib${PROJECT_NAME}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib${PROJECT_NAME}/lib)

add_library(${PROJECT_NAME}
	src/application.cpp
	src/window.cpp
	src/resources.cpp

	src/common/style.cpp

	src/renderer/glad.c
	src/renderer/font.cpp
	src/renderer/drawing_context.cpp
	src/renderer/renderer.cpp
	src/renderer/stb_image.c

	src/controls/box.cpp
	src/controls/button.cpp
	src/controls/image.cpp
	src/controls/label.cpp
	src/controls/lineedit.cpp
	src/controls/notebook.cpp
	src/controls/scrollable.cpp
	src/controls/scrollbar.cpp
	src/controls/scrolledbox.cpp
	src/controls/slider.cpp
	src/controls/spacer.cpp
	src/controls/splitter.cpp
	src/controls/treeview.cpp
	src/controls/widget.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC
	include
	include/freetype2
)

if (UNIX)
	add_executable(main src/main.cpp)
	target_link_libraries(main
		gui
		freetype
		SDL2
		GL
		X11 
		pthread 
		Xrandr 
		Xi 
		dl
	)
endif()

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
			gui ../msvc_lib/freetype ../msvc_lib/SDL2main ../msvc_lib/SDL2 opengl32
		)
	endif()
endforeach()
add_subdirectory(${PROJECT_SOURCE_DIR}/examples)

# Copy the dlls to the output dir for visual studio
if (MSVC)
	file(COPY
		${PROJECT_SOURCE_DIR}/msvc_lib/freetype.dll
		${PROJECT_SOURCE_DIR}/msvc_lib/SDL2.dll
		DESTINATION
		${EXECUTABLE_OUTPUT_PATH}
	)
endif()
