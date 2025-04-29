#include "GameWindow.h"

GameWindow::GameWindow(int width, int height, const char* title)
    : dimension(width, height) {

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init failed: %s", SDL_GetError());
    }

    // on Android, SDL_GetDisplays only works once the SDL_CreateWindow has run.
    int num_displays;
    SDL_DisplayID* displays = SDL_GetDisplays(&num_displays);
    if (displays && num_displays > 0) {
        SDL_DisplayID display_id = displays[0]; // first display

        SDL_Rect display_bounds;
        if (SDL_GetDisplayBounds(display_id, &display_bounds)) {
            int display_width = display_bounds.w;
            int display_height = display_bounds.h;
            SDL_Log("Screen size: %d x %d", display_width, display_height);
            
#if defined(__ANDROID__) || defined(__IPHONE_OS_VERSION_MIN_REQUIRED) // Android & iOS always goes for Full Screen
            dimension = glm::ivec2(display_height, display_width);
#endif
        }
        else {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to get display bounds: %s", SDL_GetError());
        }
    }
    else {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No displays found or SDL_GetDisplays failed.");
    }
    // Create window
#ifdef USE_GPU
    if (!SDL_Vulkan_LoadLibrary(NULL)) { // This should be called after initializing the video driver, but before creating any Vulkan windows
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Vulkan_LoadLibrary Error: %s", SDL_GetError());
    }
    SDL_Log("SDL: Vulkan loader and an ICD have been found");
    window = SDL_CreateWindow(title, width, height, SDL_WINDOW_VULKAN);
#else
#ifdef __ANDROID__  // Android always goes for Full Screen
    window = SDL_CreateWindow(title, width, height, SDL_WINDOW_FULLSCREEN); // SDL_WINDOW_FULLSCREEN is necessary to hide the HUDs on Android
#else
    window = SDL_CreateWindow(title, width, height, 0);
#endif
#endif
    if (window == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindow Error: %s", SDL_GetError());
    }
    SDL_SetWindowResizable(window, false);
    /* Create SDL3 renderer (pass NULL to auto-select best backend)
    *   SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    */
#ifndef USE_GPU
    renderer = SDL_CreateRenderer(window, NULL);
#endif
}

GameWindow::~GameWindow() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}
