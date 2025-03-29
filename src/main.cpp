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

    const Uint32 targetTime = 16; // Target interval in milliseconds (16ms ~ 60 FPS)
    Uint32 currentTime = SDL_GetTicks();
    std::cout << "Executing logic at " << currentTime << " ms" << std::endl;

    bool running = true;
    bool keys[SDL_SCANCODE_COUNT] = { false };
    SDL_Event event;

    while (running) {
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

    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    
    
    //initVulkan(window);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
