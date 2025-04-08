#pragma once
#include "Global.h"
#include <iostream>

#include <glm/glm.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <stdexcept>

class GameWindow {
public:
    glm::ivec2 dimension;
    SDL_Window* window;
    SDL_Renderer* renderer;

    GameWindow(int width, int height, const char* title);
    ~GameWindow();
};
