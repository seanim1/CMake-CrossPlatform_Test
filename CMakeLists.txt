cmake_minimum_required(VERSION 3.20)
# PROJECT_NAME is the name of your project
project(PROJECT_NAME)

# This ensures your project always compiles with C++14, no matter the platform or compiler
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# locates the Vulkan SDK on your system
find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIRS}) # adds this include path so that #include <vulkan/vulkan.h> works
message(STATUS "Vulkan Include Dirs: ${Vulkan_INCLUDE_DIRS}")
message(STATUS "Vulkan Libraries: ${Vulkan_LIBRARIES}")

# Find GLFW
if(WIN32)
    # Specify the path to the SDL3 folder that contains .cmake files.
    set(CMAKE_PREFIX_PATH "C:/Users/Sean/Downloads/SDL3-devel-3.2.8-VC/SDL3-3.2.8/cmake")
    # The path to your SDL3.dll file
    set(SDL3_DLL_PATH "C:/Users/Sean/Downloads/SDL3-devel-3.2.8-VC/SDL3-3.2.8/lib/x64/SDL3.dll")

    # If CMAKE_PREFIX_PATH is set:
    #   CMake will attempt to find GLFW3Config.cmake or sdl3-config.cmake in the subdirectories of CMAKE_PREFIX_PATH
    find_package(SDL3 REQUIRED)
    include_directories(${SDL3_INCLUDE_DIRS})   # Add include directory for SDL3
    message(STATUS "SDL3 Include Dirs: ${SDL3_INCLUDE_DIRS}")
    message(STATUS "SDL3 Library: ${SDL3_LIBRARIES}")
else() # Linux/macOS
	set(CMAKE_PREFIX_PATH "/opt/homebrew/include/SDL3")
    # The path to your SDL3.dll file
    set(SDL3_DYLIB_PATH "/opt/homebrew/lib/libSDL3.dylib")

    # Find SDL3
    find_package(SDL3 REQUIRED)
    include_directories(${SDL3_INCLUDE_DIRS})
    message(STATUS "SDL3 Include Dirs: ${SDL3_INCLUDE_DIRS}")
    message(STATUS "SDL3 Library: ${SDL3_LIBRARIES}")
endif()

# Add include directory so headers can be found
include_directories(${CMAKE_SOURCE_DIR}/include)

# Collect all .cpp files in the src directory as a SOURCES variable
file(GLOB SOURCES "src/*.cpp")

# EXECUTABLE_TARGET_NAME name of the compiled executable
add_executable(EXECUTABLE_TARGET_NAME ${SOURCES})

# Only add post-build commands after the target is created
if(WIN32)
    # Link SDL3 and Vulkan with your project
    target_link_libraries(EXECUTABLE_TARGET_NAME SDL3::SDL3 ${Vulkan_LIBRARIES})
    
    # Post-build command to copy the DLL to the executable directory
    add_custom_command(TARGET EXECUTABLE_TARGET_NAME POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SDL3_DLL_PATH}
        $<TARGET_FILE_DIR:EXECUTABLE_TARGET_NAME>/SDL3.dll)
endif()
if(APPLE)
    # Link SDL3 and Vulkan with your project
    target_link_libraries(EXECUTABLE_TARGET_NAME SDL3::SDL3 ${Vulkan_LIBRARIES})

    # Post-build command to copy SDL3 shared library to output directory
    add_custom_command(TARGET EXECUTABLE_TARGET_NAME POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL3_DYLIB_PATH}"
        $<TARGET_FILE_DIR:EXECUTABLE_TARGET_NAME>/libSDL3.so)

else()
    # Link SDL3 and Vulkan with your project
    target_link_libraries(EXECUTABLE_TARGET_NAME SDL3::SDL3 ${Vulkan_LIBRARIES})

    # Post-build command to copy SDL3 shared library to output directory
    add_custom_command(TARGET EXECUTABLE_TARGET_NAME POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL3_LIBRARIES}"
        $<TARGET_FILE_DIR:EXECUTABLE_TARGET_NAME>/libSDL3.so)
endif()
