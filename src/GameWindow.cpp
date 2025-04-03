#pragma once

#include "GameWindow.h"

GameWindow::GameWindow(int width, int height, const char* title)
    : dimension(width, height) {

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        const char* errorMsg = SDL_GetError();
        printf("SDL Error: %s\n", errorMsg);
        throw std::runtime_error(std::string("SDL_Init Error: ") + errorMsg);
    }
    // Create window
#ifdef USE_GPU
    if (!SDL_Vulkan_LoadLibrary(NULL)) { // This should be called after initializing the video driver, but before creating any Vulkan windows
        std::string errorMsg = std::string("SDL_Vulkan_LoadLibrary Error: ") + SDL_GetError();
        std::cout << "SDL Error: " << errorMsg << std::endl;
        throw std::runtime_error(errorMsg);
    }
    std::cout << "SDL: Vulkan loader and an ICD have been found" << std::endl;
    window = SDL_CreateWindow(title, width, height, SDL_WINDOW_VULKAN);
    // Get required Vulkan extensions
    uint32_t extensionCount = 0;
    const char* const* extensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
    printf("SDL3 Vulkan instance extensions required:\n");
    for (uint32_t i = 0; i < extensionCount; i++) {
        printf("- %s\n", extensions[i]);
    }
#else
    window = SDL_CreateWindow(title, width, height, 0);
#endif
    if (window == NULL) {
        std::string errorMsg = std::string("SDL_CreateWindow Error: ") + SDL_GetError();
        std::cout << "SDL Error: " << errorMsg << std::endl;
        throw std::runtime_error(errorMsg);
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