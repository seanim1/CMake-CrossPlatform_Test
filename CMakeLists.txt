cmake_minimum_required(VERSION 3.20)
# PROJECT_NAME is the name of your project
project(PROJECT_NAME)

# This ensures your project always compiles with C++14, no matter the platform or compiler
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# locates the Vulkan SDK on your system
find_package(Vulkan REQUIRED)
# adds this include path so that #include <vulkan/vulkan.h> works
include_directories(${Vulkan_INCLUDE_DIRS})
message(STATUS "Vulkan Include Dirs: ${Vulkan_INCLUDE_DIRS}")
message(STATUS "Vulkan Libraries: ${Vulkan_LIBRARIES}")

# Find GLFW
if(WIN32)
    # Specify the path to the SDL3 folder that contains .cmake files.
    set(CMAKE_PREFIX_PATH "C:/Users/Sean/Downloads/SDL3-devel-3.2.8-VC/SDL3-3.2.8/cmake")

    # If CMAKE_PREFIX_PATH is set:
    #   CMake will attempt to find GLFW3Config.cmake or glfw3-config.cmake in the subdirectories of CMAKE_PREFIX_PATH
    # looks like "GLFW3" only works on WIN32, and "GLFW" only works on Linux...
    
    # Find SDL3 automatically
    find_package(SDL3 REQUIRED)

    # Add include directory for SDL3
    include_directories(${SDL3_INCLUDE_DIRS})
    message(STATUS "SDL3 Include Dirs: ${SDL3_INCLUDE_DIRS}")
    message(STATUS "SDL3 Library: ${SDL3_LIBRARIES}")
else() # Linux/macOS

    # It adds "${CMAKE_SOURCE_DIR}" to CMAKE_MODULE_PATH, allowing CMake to search for .cmake files in the source directory
    # Make sure CMake finds FindGLFW.cmake
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")

    # When find_package(GLFW REQUIRED) is called:
    #   1. CMake first looks in CMAKE_MODULE_PATH for FindGLFW.cmake
    #   2. If found, it runs FindGLFW.cmake, which uses find_path() and find_library() to locate GLFW.
    # Find GLFW using FindGLFW.cmake
    find_package(GLFW REQUIRED)
endif()

# Add include directory so headers can be found
include_directories(${CMAKE_SOURCE_DIR}/include)

# Collect all .cpp files in the src directory as a SOURCES variable
file(GLOB SOURCES "src/*.cpp")

# EXECUTABLE_TARGET_NAME name of the compiled executable
add_executable(EXECUTABLE_TARGET_NAME ${SOURCES})

# Only add post-build commands after the target is created
if(WIN32)
    # Link GLFW and Vulkan with your project
    target_link_libraries(EXECUTABLE_TARGET_NAME SDL3::SDL3 ${Vulkan_LIBRARIES})
    # The path to your glfw3.dll file
    set(SDL3_DLL_PATH "C:/Users/Sean/Downloads/SDL3-devel-3.2.8-VC/SDL3-3.2.8/lib/x64/SDL3.dll")

    # Post-build command to copy the DLL to the executable directory
    add_custom_command(TARGET EXECUTABLE_TARGET_NAME POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SDL3_DLL_PATH}
        $<TARGET_FILE_DIR:EXECUTABLE_TARGET_NAME>/SDL3.dll)
else()
    # Include GLFW headers
    include_directories(${GLFW_INCLUDE_DIR})

    # Link GLFW and Vulkan with your project
    target_link_libraries(EXECUTABLE_TARGET_NAME ${GLFW_LIBRARY} ${Vulkan_LIBRARIES})

    add_custom_command(TARGET EXECUTABLE_TARGET_NAME POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${GLFW_LIBRARY}"
        $<TARGET_FILE_DIR:EXECUTABLE_TARGET_NAME>/libglfw.so)
endif()
