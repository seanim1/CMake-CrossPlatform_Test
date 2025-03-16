cmake_minimum_required(VERSION 3.20)
# PROJECT_NAME is the name of your project
project(PROJECT_NAME)

# This ensures your project always compiles with C++17, no matter the platform or compiler
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directory so headers can be found
include_directories(${CMAKE_SOURCE_DIR}/include)

# EXECUTABLE_TARGET_NAME name of the compiled executable
add_executable(EXECUTABLE_TARGET_NAME src/main.cpp)
