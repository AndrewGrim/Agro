cmake_minimum_required(VERSION 3.16.3)
cmake_policy(SET CMP0091 NEW)

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
	set(CMAKE_CXX_FLAGS_DEBUG "/DEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE "/O2")
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/lib)

file(GLOB sources
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/*.hpp"
	"${PROJECT_SOURCE_DIR}/src/common/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/common/*.hpp"
	"${PROJECT_SOURCE_DIR}/src/controls/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/controls/*.hpp"
	"${PROJECT_SOURCE_DIR}/src/renderer/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/renderer/*.c"
	"${PROJECT_SOURCE_DIR}/src/renderer/*.hpp"
	"${PROJECT_SOURCE_DIR}/src/renderer/*.h"
)
add_library(${PROJECT_NAME} ${sources})
target_include_directories(${PROJECT_NAME} PUBLIC
	include
	include/freetype2
)
add_subdirectory(${PROJECT_SOURCE_DIR}/tests)
add_subdirectory(${PROJECT_SOURCE_DIR}/examples)

# Install the lib and header files
if (UNIX)
	install(
		TARGETS ${PROJECT_NAME}
		ARCHIVE DESTINATION lib
	)
	file(GLOB root "${PROJECT_SOURCE_DIR}/src/*.hpp")
	install(
		FILES ${root}
		DESTINATION include/${PROJECT_NAME}
	)
	file(GLOB common "${PROJECT_SOURCE_DIR}/src/common/*.hpp")
	install(
		FILES ${common}
		DESTINATION include/${PROJECT_NAME}/common
	)
	file(GLOB controls "${PROJECT_SOURCE_DIR}/src/controls/*.hpp")
	install(
		FILES ${controls}
		DESTINATION include/${PROJECT_NAME}/controls
	)
	file(GLOB renderer "${PROJECT_SOURCE_DIR}/src/renderer/*.hpp" "${PROJECT_SOURCE_DIR}/src/renderer/*.h")
	install(
		FILES ${renderer}
		DESTINATION include/${PROJECT_NAME}/renderer
	)
endif()
