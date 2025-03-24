# CMake GUI skeleton for Windows, Linux, MacOS
Display processor specs then begin a GUI.
## Installing GLFW for CMake

### Windows: Installing GLFW

- [Install vcpkg, a C++ package manager](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-powershell)
- Open terminal (Command Prompt or Powershell).
- Install glfw3 64-bit.
   ```bash
   vcpkg install glfw3:x64-windows
- Now you should be able to locate
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
