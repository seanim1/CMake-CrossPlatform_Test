# Vulkan & SDL3 skeleton for Windows, Linux, MacOS, Android
- SDL3: begin a SDL3 window, handle inputs.
- Vulkan: Query extensions. Create a VkSurface.
## Install Vulkan SDK
- For Windows, Linux, MacOS, [LunarG Vulkan SDK](https://vulkan.lunarg.com/)
- For Android, its SDK should have it already installed.
## Including and Linking SDL3 for development
### Windows
- [Download SDL](https://github.com/libsdl-org/SDL/releases)
- Download `SDL3-devel-3.2.8-VC.zip` for Visual Studio C++
- Example: you should be able to locate
   - .cmake at: "\Downloads\SDL3-devel-3.2.8-VC\SDL3-3.2.8\cmake"
   - Header at: "\Downloads\SDL3-devel-3.2.8-VC\SDL3-3.2.8\include"
   - Dynamic Library at: "\Downloads\SDL3-devel-3.2.8-VC\SDL3-3.2.8\lib\x64\SDL3.dll"
- Open CMakeLists.txt file and set the paths accordingly
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
- Example: you should be able to locate
   - Header at: "/Downloads/SDL/include/SDL3"
   - Dynamic Library at: "/Downloads/SDL/build/libSDL3.so"
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
 
 ### Android
- I decided to start from boilerplate from SDL GitHub repo (I would suggest cloning to a folder with a short path name. As a command too big can fail the build)
   - [SDL-GitHub Repo](https://github.com/libsdl-org/SDL/blob/main/docs/INTRO-androidstudio.md)
- That is all. You should notice that in Android.mk file (SDL\build\org.libsdl.hello\app\jni\src\Android.mk), including and linking of SDL3 has already been done.
## Run CMake:

