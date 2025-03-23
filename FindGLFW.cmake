# Copyright (c) 2015 Andrew Kelley
# This file is MIT licensed.
# See http://opensource.org/licenses/MIT

# GLFW_FOUND
# GLFW_INCLUDE_DIR
# GLFW_LIBRARY

# This searches for the directory that contains the GLFW header file (glfw3.h).
# It sets GLFW_INCLUDE_DIR to the directory where GLFW/glfw3.h is found
find_path(GLFW_INCLUDE_DIR NAMES GLFW/glfw3.h)

# This searches for the GLFW library file (libglfw.so on Linux, glfw3.lib on Windows).
# It sets GLFW_LIBRARY to the full path of the GLFW library.
find_library(GLFW_LIBRARY NAMES glfw glfw3)

# This includes a CMake module that provides standard argument handling for find_package()
include(FindPackageHandleStandardArgs)
# This ensures that both GLFW_LIBRARY and GLFW_INCLUDE_DIR were successfully found
find_package_handle_standard_args(GLFW DEFAULT_MSG GLFW_LIBRARY GLFW_INCLUDE_DIR)

## This tells CMake that these variables (GLFW_INCLUDE_DIR and GLFW_LIBRARY) should be hidden from normal users in the CMake GUI
#mark_as_advanced(GLFW_INCLUDE_DIR GLFW_LIBRARY)
