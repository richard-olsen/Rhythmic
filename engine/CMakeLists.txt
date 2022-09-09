project(WarmfulsEngine CXX)

file(GLOB_RECURSE WE_FILES_SRCS "${PROJECT_SOURCE_DIR}/src/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE WE_FILES_HDRS "${PROJECT_SOURCE_DIR}/include/*.hpp"
	"${PROJECT_SOURCE_DIR}/include/*.h")

include_directories(
	"${PROJECT_SOURCE_DIR}/include/"
	${RHYTHM_INCLUDES}
)

add_library(wge STATIC ${WE_FILES_SRCS} ${WE_FILES_HDRS})
target_link_libraries(
	wge
	${RHYTHM_LIBRARIES}
)
