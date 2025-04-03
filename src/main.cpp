#include "Global.h"
#include "GameWindow.h"
#include "GameScreen.h"
#include "GameInput.h"

#include "Vulkan_gpu.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <glm/glm.hpp>

#ifdef _WIN32
#include "Windows_Specific.h"
#elif __linux__
#include "Linux_Specific.h"
#elif __APPLE__
#include "MacOS_Specific.h"
#endif

//#define USE_GPU
#define LOCK_60FPS

int main() {
    CPU_CAPABILITIES cpuCaps = queryProcessor_PLATFORM();
    glm::ivec2 Window_Dim = glm::ivec2(800, 600);
    glm::ivec2 Screen_Dim = glm::ivec2(80, 60);
    GameWindow* gWindow = new GameWindow(Window_Dim.x, Window_Dim.y, "Cross-Platform GUI");
    GameScreen* gScreen = new GameScreen(Screen_Dim.x, Screen_Dim.y, gWindow->renderer);
    GameInput* gInput = new GameInput();
#ifdef USE_GPU
    initVulkan(gWindow->window);
#endif
    bool running = true;
#ifdef LOCK_60FPS
#define TARGET_FRAMETIME_MICROSECOND 16667 // 16.67 ms
    int64_t hardwareTimerFrequency = SDL_GetPerformanceFrequency(); // equivalent to WIN32 QueryPerformanceCounter. hardware counter frequency. Its value is usually 10 Mhz / 10,000,000 hz (100 nanosecond unit)
    constexpr int64_t sec_to_microsecond = (1000000 + 1);
    const int64_t target_FrameTime_HardwareSec = TARGET_FRAMETIME_MICROSECOND * hardwareTimerFrequency / sec_to_microsecond;
    float deltaTime = 0;  // Stores previous frame's timestamp
    float elapsedTime = 0; // total time since program start in seconds
#endif
    while (running) {
        int64_t frameStartTime_HardwareSec = SDL_GetPerformanceCounter();
        gInput->Update();
#ifdef USE_GPU
#else
        gScreen->Update(elapsedTime, gWindow->renderer);
#endif
#ifdef LOCK_60FPS
        int64_t frameEndTime_HardwareSec = SDL_GetPerformanceCounter();
        // HardwareSec (assume it will be less than MicroSecond) -> ScaledHardwareSec -> MicroSecond
        int64_t cpuWorkTime_HardwareSec = (frameEndTime_HardwareSec - frameStartTime_HardwareSec);
#if 0
            float cpuWorkTime_Sec = (float) cpuWorkTime_HardwareSec / hardwareTimerFrequency;
            printf("cpuWorkTime_Sec: %.8f\n", cpuWorkTime_Sec);
            printf("DeltaTime: %.8f\n", deltaTime);
            printf("ElapsedTime: %.8f\n", elapsedTime);
#endif
        // if we are done sooner than 16ms, then do busy waiting.
        while (cpuWorkTime_HardwareSec < target_FrameTime_HardwareSec) {
            frameEndTime_HardwareSec = SDL_GetPerformanceCounter();
            cpuWorkTime_HardwareSec = (frameEndTime_HardwareSec - frameStartTime_HardwareSec);
            // If we are less than 75% of the way through the current frame, then rest for 1 ms.
            if (cpuWorkTime_HardwareSec < (target_FrameTime_HardwareSec * (1. - (2. / 16.)))) {
                SDL_Delay(1);
            }
        }
        frameEndTime_HardwareSec = SDL_GetPerformanceCounter();
        cpuWorkTime_HardwareSec = (frameEndTime_HardwareSec - frameStartTime_HardwareSec);
        deltaTime = (float) cpuWorkTime_HardwareSec / hardwareTimerFrequency;
        elapsedTime += deltaTime;
#endif
    }
    delete gScreen;
    delete gWindow;
    return 0;
}
