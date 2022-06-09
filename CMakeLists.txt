cmake_minimum_required(VERSION 3.16.3)
cmake_policy(SET CMP0091 NEW)

project(Agro)

option(BUILD_ALL_TESTS "Build all test binaries" OFF)
option(BUILD_ALL_EXAMPLES "Build all example binaries" OFF)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-parameter -Wpedantic -no-pie")
	set(CMAKE_CXX_FLAGS_DEBUG "-g")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set(CMAKE_CXX_FLAGS_DEBUG "/DEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE "/O2")
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()

file(GLOB sources
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/*.hpp"
	"${PROJECT_SOURCE_DIR}/src/common/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/common/*.hpp"
	"${PROJECT_SOURCE_DIR}/src/controls/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/controls/*.hpp"
	"${PROJECT_SOURCE_DIR}/src/core/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/core/*.hpp"
	"${PROJECT_SOURCE_DIR}/src/renderer/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/renderer/*.c"
	"${PROJECT_SOURCE_DIR}/src/renderer/*.hpp"
	"${PROJECT_SOURCE_DIR}/src/renderer/*.h"
)
add_library(${PROJECT_NAME} ${sources})
target_include_directories(${PROJECT_NAME} PUBLIC
	SDL2-2.0.16/include
	freetype-2.11.0/include
)
target_link_directories(${PROJECT_NAME} PUBLIC
	"${PROJECT_BINARY_DIR}/SDL2-2.0.16"
	"${PROJECT_BINARY_DIR}/Debug/SDL2-2.0.16"
	"${PROJECT_BINARY_DIR}/Release/SDL2-2.0.16"
	"${PROJECT_BINARY_DIR}/freetype-2.11.0"
	"${PROJECT_BINARY_DIR}/Debug/freetype-2.11.0"
	"${PROJECT_BINARY_DIR}/Release/freetype-2.11.0"
)
if (APPLE)
	target_link_libraries(${PROJECT_NAME}
		freetype
		SDL2main
		$<$<CONFIG:DEBUG>:SDL2d>
		$<$<CONFIG:RELEASE>:SDL2>
		"-framework OpenGL"
		"-lm"
		"-liconv"
		"-framework CoreAudio"
		"-framework AudioToolbox"
		"-weak_framework CoreHaptics"
		"-weak_framework GameController"
		"-framework ForceFeedback"
		"-lobjc"
		"-framework CoreVideo"
		"-framework Cocoa"
		"-framework Carbon"
		"-framework IOKit"
		"-weak_framework QuartzCore"
		"-weak_framework Metal"
	)
elseif (MSVC)
	target_link_libraries(${PROJECT_NAME}
		freetype
		SDL2main
		$<$<CONFIG:DEBUG>:SDL2d>
		$<$<CONFIG:RELEASE>:SDL2>
		opengl32
		winmm
		imm32
		version
		setupapi
	)
else ()
	target_link_libraries(${PROJECT_NAME}
		freetype
		SDL2main
		SDL2
		GL
		X11
		pthread
		Xrandr
		Xi
		dl
	)
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/lib"
	ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/lib"
	ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/lib"
)

set(BUILD_SHARED_LIBS OFF)
set(HAVE_LIBC ON)
add_subdirectory(${PROJECT_SOURCE_DIR}/SDL2-2.0.16)

set(CMAKE_DISABLE_FIND_PACKAGE_ZLIB TRUE)
set(CMAKE_DISABLE_FIND_PACKAGE_BZip2 TRUE)
set(CMAKE_DISABLE_FIND_PACKAGE_PNG TRUE)
set(CMAKE_DISABLE_FIND_PACKAGE_HarfBuzz TRUE)
set(CMAKE_DISABLE_FIND_PACKAGE_BrotliDec TRUE)
add_subdirectory(${PROJECT_SOURCE_DIR}/freetype-2.11.0)
set(FT_WITH_ZLIB OFF)
set(FT_WITH_BZIP2 OFF)
set(FT_WITH_PNG OFF)
set(FT_WITH_HARFBUZZ OFF)
set(FT_WITH_BROTLI OFF)

add_subdirectory(${PROJECT_SOURCE_DIR}/tests)
add_subdirectory(${PROJECT_SOURCE_DIR}/examples)

# TODO Should happen at build time
# Copy the header files into the include folder
file(GLOB root "${PROJECT_SOURCE_DIR}/src/*.hpp")
file(
	COPY ${root}
	DESTINATION ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/include/${PROJECT_NAME}
)
file(GLOB common "${PROJECT_SOURCE_DIR}/src/common/*.hpp" "${PROJECT_SOURCE_DIR}/src/common/*.h")
file(
	COPY ${common}
	DESTINATION ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/include/${PROJECT_NAME}/common
)
file(GLOB controls "${PROJECT_SOURCE_DIR}/src/controls/*.hpp")
file(
	COPY ${controls}
	DESTINATION ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/include/${PROJECT_NAME}/controls
)
file(GLOB core "${PROJECT_SOURCE_DIR}/src/core/*.hpp")
file(
	COPY ${core}
	DESTINATION ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/include/${PROJECT_NAME}/core
)
file(GLOB renderer "${PROJECT_SOURCE_DIR}/src/renderer/*.hpp" "${PROJECT_SOURCE_DIR}/src/renderer/*.h")
file(
	COPY ${renderer}
	DESTINATION ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/include/${PROJECT_NAME}/renderer
)

# Install the lib and header files
if (UNIX)
	install(
		TARGETS ${PROJECT_NAME}
		ARCHIVE DESTINATION lib
	)
	install(
		DIRECTORY ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/include/
		DESTINATION include
	)
endif()
