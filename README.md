# SDL3 & Vulkan skeleton for Windows, Linux, MacOS, Android
- SDL3: begin a SDL3 window, handle inputs, start CPU rendering game loop.
- Vulkan: Query extensions. Create a VkSurface.
## Integrate SDL3
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
## Special case for Android SDK
### Android
- Drag the "include" and "src" folders in this repository into SDL GitHub repo's subdirectory: (SDL\build\org.libsdl.hello\app\jni\src). Such that you would have (SDL\build\org.libsdl.hello\app\jni\src\src) and (SDL\build\org.libsdl.hello\app\jni\src\include).
- In Android.mk,
   - change LOCAL_SRC_FILES with
   - ```make
     LOCAL_SRC_FILES := $(wildcard src/*.cpp)
   - change LOCAL_C_INCLUDES with
   - ```make
     LOCAL_C_INCLUDES := \
      $(LOCAL_PATH)/$(SDL_PATH)/include \
      $(LOCAL_PATH)/include
- In Android.mk, add the following to compile in C++:
   - ```make
     APP_CPPFLAGS += -fexceptions
     APP_STL := c++_shared
## Integrate Vulkan SDK
### Windows, Linux, MacOS
- For Windows, Linux, MacOS, just download the SDK. [LunarG Vulkan SDK](https://vulkan.lunarg.com/)
- CMakeLists.txt appears to be capable of finding the library automatically. If not, you can always specify it yourself.
### Android
- For Android, its SDK should have Vulkan SDK already installed, but the path to the dynamic library has to be specified.
- In Android.mk, 
- ```make
   VULKAN_LIB_PATH := $(NDK_ROOT)/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/lib/aarch64-linux-android/28
- *(28 here is the Android SDK API version, equivalent to Vulkan 1.1), then add,
- ```make
   LOCAL_LDLIBS += -L$(VULKAN_LIB_PATH) -lvulkan
- You might want to go to Application.mk and App/build.gradle and specify the targetSdkVersion numbers accordingly.
## Build:
### Windows (Visual Studio), Linux, MacOS (XCode)
- Use CMake GUI (or CMake command line) to configure, generate, open project. build & run ALL_BUILD first, then build & run the Project. 
### Android (Android Studio)
- In Android Studio (if you aren't in it already), connect with your Android device then press "Run".
