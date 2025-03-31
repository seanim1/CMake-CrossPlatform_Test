#include "Global.h"
#include "GLFW_gui.h"
#include "Vulkan_gpu.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#ifdef _WIN32
#include "Windows_Specific.h"
#elif __linux__
#include "Linux_Specific.h"
#elif __APPLE__
#include "MacOS_Specific.h"
#endif

#define LOCK_60FPS

int main() {
    queryProcessor_PLATFORM();
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    if (!SDL_Vulkan_LoadLibrary(NULL)) { // This should be called after initializing the video driver, but before creating any Vulkan windows
        std::cerr << "SDL_Vulkan_LoadLibrary failed: " << SDL_GetError() << std::endl;
        return -1;
    }
    std::cout << "SDL: Vulkan loader and an ICD have been found" << std::endl;

    SDL_Window* window = SDL_CreateWindow("Cross-Platform GUI", 800, 600, SDL_WINDOW_VULKAN);
    SDL_SetWindowResizable(window, false);
    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        return -1;
    }
    // Get required Vulkan extensions
    uint32_t extensionCount = 0;
    const char* const* extensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
    printf("SDL3 Vulkan instance extensions required:\n");
    for (uint32_t i = 0; i < extensionCount; i++) {
        printf("- %s\n", extensions[i]);
    }

    initVulkan(window);

    bool running = true;
    bool keys[SDL_SCANCODE_COUNT] = { false };
    SDL_Event event;
#ifdef LOCK_60FPS
#define TARGET_FRAMETIME_MICROSECOND 16667 // 16.67 ms
    int64_t hardwareTimerFrequency = SDL_GetPerformanceFrequency(); // equivalent to WIN32 QueryPerformanceCounter. hardware counter frequency. Its value is usually 10 Mhz / 10,000,000 hz (100 nanosecond unit)
    constexpr int64_t sec_to_microsecond = (1000000 + 1);
    const int64_t target_FrameTime_HardwareSec = TARGET_FRAMETIME_MICROSECOND * hardwareTimerFrequency / sec_to_microsecond;
    float deltaTime = 0;  // Stores previous frame's timestamp
    float elapsedTime = 0;
#endif
    while (running) {
        int64_t frameStartTime_HardwareSec = SDL_GetPerformanceCounter();
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false; // Exit the loop on quit event
            }
            if (event.type == SDL_EVENT_KEY_DOWN) {
                // Mark key as pressed
                keys[event.key.scancode] = true;
                std::cout << "Key pressed: " << SDL_GetScancodeName(event.key.scancode) << std::endl;
            }
            else if (event.type == SDL_EVENT_KEY_UP) {
                // Mark key as released
                keys[event.key.scancode] = false;
                std::cout << "Key released: " << SDL_GetScancodeName(event.key.scancode) << std::endl;
            }
        }
        // Example of checking if a specific key is held down
        if (keys[SDL_SCANCODE_W]) std::cout << "Holding W" << std::endl;
        if (keys[SDL_SCANCODE_A]) std::cout << "Holding A" << std::endl;
        if (keys[SDL_SCANCODE_S]) std::cout << "Holding S" << std::endl;
        if (keys[SDL_SCANCODE_D]) std::cout << "Holding D" << std::endl;
#ifdef LOCK_60FPS
        int64_t frameEndTime_HardwareSec = SDL_GetPerformanceCounter();
        // HardwareSec (assume it will be less than MicroSecond) -> ScaledHardwareSec -> MicroSecond
        int64_t cpuWorkTime_HardwareSec = (frameEndTime_HardwareSec - frameStartTime_HardwareSec);
#if 1
            float cpuWorkTime_Sec = (float) cpuWorkTime_HardwareSec / hardwareTimerFrequency;
            printf("cpuWorkTime_Sec: %.8f\n", cpuWorkTime_Sec);
            printf("DeltaTime: %.8f\n", deltaTime);
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

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
