# Vulkan & SDL3 skeleton for Windows, Linux, MacOS
- Platform Independent: Display CPU specs.
- SDL3: begin a SDL3 window, handle inputs.
- Vulkan: Query extensions. Create a VkSurface.
## Install Vulkan SDK
- [LunarG Vulkan SDK](https://vulkan.lunarg.com/)
## Installing SDL3 for development

### Windows
- [Download SDL](https://github.com/libsdl-org/SDL/releases)
- Download `SDL3-devel-3.2.8-VC.zip` for Visual Studio C++
- you should be able to locate
   - .cmake (glfw3/GLFW3Config.cmake) at: "C:\vcpkg\installed\x64-windows\share\glfw3"
   - Header (GLFW/glfw3.h) at: "C:\vcpkg\installed\x64-windows\include"
   - Dynamic Library at: "C:\vcpkg\installed\x64-windows\bin\glfw3.dll"
### Linux (Ubuntu)
- Open terminal (Bash).
- I had to build SDL from source
   ```bash
   git clone https://github.com/libsdl-org/SDL.git
   cd SDL
   mkdir build && cd build
   cmake .. -DCMAKE_INSTALL_PREFIX=/usr
   make -j$(nproc)
   sudo make install

- Open CMakeLists.txt file and set the paths accordingly

### MacOS
- Install Homebrew.
- Open terminal (Bash).
- Install SDL3.
   ```bash
   brew install sdl3
- Now you should be able to locate
   - Headers (SDL3/SDL.h) at: "/opt/homebrew/include"
   - Dynamic Library at: "/opt/homebrew/lib/libSDL3.dylib"
- Open CMakeLists.txt file and set the paths accordingly. (Example)
   - set(CMAKE_PREFIX_PATH "/opt/homebrew/include/SDL3")
   - set(SDL3_DYLIB_PATH "/opt/homebrew/lib/libSDL3.dylib")
## Run CMake:

