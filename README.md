# Vulkan & SDL3 skeleton for Windows, Linux, MacOS
- Platform Independent: Display CPU specs.
- SDL3: begin a SDL3 window, handle inputs.
- Vulkan: Query extensions. Create a VkSurface.
## Install Vulkan SDK
- [LunarG Vulkan SDK](https://vulkan.lunarg.com/)
## Installing GLFW for CMake

### Windows
- [Download SDL](https://github.com/libsdl-org/SDL/releases)
- Download `SDL3-devel-3.2.8-VC.zip` for Visual Studio C++
- you should be able to locate
   - .cmake (glfw3/GLFW3Config.cmake) at: "C:\vcpkg\installed\x64-windows\share\glfw3"
   - Header (GLFW/glfw3.h) at: "C:\vcpkg\installed\x64-windows\include"
   - Dynamic Library at: "C:\vcpkg\installed\x64-windows\bin\glfw3.dll"
### Linux (Ubuntu)
- Open terminal (Bash).
- Install glfw.
   ```bash
   sudo apt-get install libglfw3 libglfw3-dev
- Now you should be able to locate
   - .cmake (glfw3/GLFW3Config.cmake) at: "/usr/lib/x86_64-linux-gnu/cmake/glfw3"
   - Header (GLFW/glfw3.h) at: "/usr/include"
   - Dynamic Library at: "/usr/lib/x86_64-linux-gnu/libglfw.so"

### MacOS
- Install Homebrew.
- Open terminal (Bash).
- Install glfw.
   ```bash
   brew install glfw
- Now you should be able to locate
   - Header (GLFW/glfw3.h) at: "/opt/homebrew/include"
   - Dynamic Library at: "/opt/homebrew/lib/libglfw.dylib"

## Run CMake:
### Windows
- If the CMake build fails to find glfw3.dll, then you might have to manually type in the path to the glfw3.dll in CMakeLists.txt.
### Linux & MacOS
- CMake should be able to find the header and library automatically.
