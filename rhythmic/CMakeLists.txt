project(Rhythmic CXX)

file(GLOB_RECURSE FILES_SRCS "${PROJECT_SOURCE_DIR}/src/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE FILES_HDRS "${PROJECT_SOURCE_DIR}/src/*.hpp"
	"${PROJECT_SOURCE_DIR}/src/*.h")

set (FILES_MISC "")

if (WIN32)
	set (FILES_MISC "${PROJECT_SOURCE_DIR}/windows.rc")
endif ()

set (PLATFORM_LIBS "")

if (CMAKE_BUILD_TYPE STREQUAL Release OR CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)
	set (CMAKE_WIN32_EXECUTABLE ON)
endif ()


include_directories("${WarmfulsEngine_SOURCE_DIR}/include/" ${RHYTHM_INCLUDES})

add_executable(rhythmic ${FILES_SRCS} ${FILES_HDRS} ${FILES_MISC})
target_link_libraries(
rhythmic
wge
${RHYTHM_LIBRARIES}
) # ${ASSIMP_LIBRARIES}) #${FMOD_LIBRARY}

