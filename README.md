# SDL3 & Vulkan 1.1+ skeleton for Windows, Linux, MacOS, Android, iOS
![7_Cube](https://github.com/user-attachments/assets/67d50b3e-195c-4c17-8124-5badf19cac16)
- Renders rotating triangle in real-time on GPU.
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
   - [SDL-GitHub Repo: Android Studio](https://github.com/libsdl-org/SDL/blob/main/docs/INTRO-androidstudio.md)
- That is all. You should notice that in Android.mk file (SDL\build\org.libsdl.hello\app\jni\src\Android.mk), including and linking of SDL3 has already been done.
### iOS
- We start from boilterplate from SDL GitHub repo [SDL-GitHub Repo: XCode](https://github.com/libsdl-org/SDL/blob/main/docs/INTRO-xcode.md). Follow the instructures here.
- I found the instructions a little bit confusing, look out for the following:
   - I am just gonna ignore this: Remove the main storyboard that was automatically added to the project
   - Where it says right click the project, we refer to the one with triangle icon.
   - as for "hello.c," “copy files to destination”
   - as for "SDL.xcodeproj." "link by reference"
## Integrate our source, headers, and shaders
### Windows, Linux, MacOS
- Handled by CMake
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
- In Application.mk, add the following to compile in C++:
   - ```make
     APP_CPPFLAGS += -fexceptions
     APP_STL := c++_shared
- Shader Binary files:
   - [Load in-app content](https://developer.android.com/develop/ui/views/layout/webapps/load-local-content)
      - In Android Studio, right-click the app > src > main folder and then choose New > Directory.
      - Name the folder "assets"
   - from "shaderBinary" folder, move all the .spv files to the "app/src/main/assets"
### iOS
- Source file:
   - Drag in the “src” folder from my repository into the new project (Action: Reference files in place). XCode should automatically be able to find the src files.
- Include files:
   - it is important that you do not drag in the include folder into the new project.
   - select the project in the navigator, click on your target under the “Targets”, go to the “Build Settings” tab, search for “Search Paths” -> “Header Search Paths”, in the “Header Search Paths”, specify the path to the include folder, .h files should all be accessible in the XCode editor .cpp files.
- Shader Binary files:
   - Go into "shaderBinary" folder.
   - Select all the .spv files, and drag it into the XCode project. (it is important you do not drag the "shaderBinary" folder in, but each individual .spv files)
## Integrate Vulkan SDK
### Windows, Linux, MacOS
- For Windows, Linux, MacOS, just download the respective SDK. [LunarG Vulkan SDK](https://vulkan.lunarg.com/)
- CMakeLists.txt appears to be capable of finding the library automatically. If not, you can always specify it yourself.
### Android
- For Android, its SDK should have Vulkan SDK already installed, but the path to the dynamic library has to be specified.
- In Android.mk, 
- ```make
   VULKAN_LIB_PATH := $(NDK_ROOT)/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/lib/aarch64-linux-android/33
- *(28 here is the Android SDK API version, equivalent to Vulkan 1.1), We need 33+, since 33 is when Vulkan 1.3 is supported. then add,
- ```make
   LOCAL_LDLIBS += -L$(VULKAN_LIB_PATH) -lvulkan
- You might want to go to Application.mk and App/build.gradle and specify the targetSdkVersion numbers accordingly.
### iOS
- Download the MacOS version of the Vulkan SDK. [LunarG Vulkan SDK](https://vulkan.lunarg.com/)
- Header:
   - in the “Header Search Paths”, specify the path to the include folder, "/VulkanSDK/{version}/macOS/header"
- Library (dynamic linking):
   - go to /VulkanSDK/{version}/macOS/lib/MoltenVK.xcframework
   - drag in the MoltenVK.xcframework into the project, anywhere within the project’s XCode directory should be fine.
   - Now select the project, select the main target, select “Build Phases”, look for "Link Binary With Libraries". You should see the “MoltenVK.xcframework” available here. Once you try to build, you will notice that errors show up, which should be fixed by adding the following frameworks from Apple SDKs:
      - CoreFoundation.framework
      - Metal.framework
      - IOSurface.framework
      - CoreGraphics.framework
      - Foundation.framework
      - QuartzCore.framework
      - UIKit.framework

## Build:
### Windows (Visual Studio), Linux, MacOS (XCode)
- Use CMake GUI (or CMake command line) to configure, generate, open project. build & run ALL_BUILD first, then build & run the Project. 
### Android (Android Studio)
- [Enable the developer mode](https://developer.android.com/studio/debug/dev-options)
   - Tapping the Build Number 7 times.
- [Run the project on Android Device](https://developer.android.com/studio/run/device)
   - On the device, open the Settings app, select Developer options, and then enable USB debugging. (you may need to turn it off and back on)
- In Android Studio (if you aren't in it already), connect with your Android device then press "Run".
### iOS (XCode)
- try to build the project.
- go to Settings -> Privacy & Security, enable the Developer Mode on your iPhone.
- go to Settings -> VPN & Device Management, and trust the app.
