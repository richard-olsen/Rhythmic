cmake_minimum_required(VERSION 3.3)

project(RhythmicBase CXX)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/modules")
set (CMAKE_CXX_STANDARD 17)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set (WGE_DEBUG 1)
endif ()

if (WGE_USE_32BIT)
	add_definitions(-DWGE_32BIT)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
else ()
	add_definitions(-DWGE_64BIT)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin64")
endif ()

if (WGE_DEBUG)
	add_definitions(-DWGE_DEBUG)
	message ("Using Debug Build of Rhythmic")
endif ()

if (WIN32)
	if (NOT WGE_DEBUG)
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
	endif ()

	set (GLFW_USE_STATIC_LIBS ON)
	
	set (PLATFORM_LIBS wsock32 ws2_32)
else ()
	set (
		EXTRA_LIBS
		pthread
		uuid
	)
endif()

find_package(OpenGL REQUIRED)
find_package(GLM REQUIRED)
find_package(GLFW REQUIRED)

find_library(
		BASS_LIBRARY
		NAMES bass
		PATHS
		${PROJECT_SOURCE_DIR}/libs/64)

set (RHYTHM_INCLUDES "${PROJECT_SOURCE_DIR}/libs/include" ${GLFW_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS})
set (RHYTHM_LIBRARIES ${BASS_LIBRARY} ${EXTRA_LIBS} ${GLFW_LIBRARIES} ${CMAKE_DL_LIBS} ${OPENGL_LIBRARIES} ${PLATFORM_LIBS})

message ("Configuring Engine and Rhythmic")

add_subdirectory(engine)
add_subdirectory(rhythmic)

