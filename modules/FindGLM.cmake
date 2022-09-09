# Find GLM
#
# Defines
# - GLM_INCLUDE_DIRS
#
# GLM_ROOT_DIR
# 	- Root directory of GLM

set(GLM_DIR_SEARCH
	"${GLM_ROOT_DIR}"
	"/usr/include"
	"/usr/local/include")

find_path(
	GLM_INCLUDE_DIRS
	NAMES glm/glm.hpp
	PATHS ${GLM_DIR_SEARCH})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLM DEFAULT_MSG GLM_INCLUDE_DIRS)
