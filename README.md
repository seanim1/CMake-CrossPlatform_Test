# CMake GLFW skeleton for Windows, Linux, MacOS

## Installing GLFW for CMake

using vcpkg to download glfw (make sure you have installed Git though)

### Windows: Installing GLFW

First, you need to install `vcpkg`, a C++ package manager.

1. Open **Command Prompt** or **PowerShell**.
2. Clone the `vcpkg` repository in your C drive root:

   ```bash
   cd \
   git clone https://github.com/microsoft/vcpkg.git
3. Navigate to the `vcpkg` directory and execute the bootstrap script:

   ```bash
   cd vcpkg
   .\bootstrap-vcpkg.bat
4. Clone the `vcpkg` repository in your C drive root:

   ```bash
   vcpkg install glfw3:x64-windows
5. Now you should be able to locate .cmake files (like GLFW3Config.cmake) at "C:\vcpkg\installed\x64-windows\share\glfw3" and "glfw3.dll" at "C:\vcpkg\installed\x64-windows\bin"
### Linux (Ubuntu): Installing GLFW
1. Open terminal **Bash**.
   ```bash
   sudo apt-get install libglfw3 libglfw3-dev
2. Now you should be able to locate .cmake files (like GLFW3Config.cmake) at "/usr/lib/x86_64-linux-gnu/cmake/glfw3" and "libglfw.so" at "/usr/lib/x86_64-linux-gnu/libglfw.so"
## Run CMake:

