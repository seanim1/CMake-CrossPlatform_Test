#pragma once
#include <iostream>

#include <glm/glm.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <stdexcept>

class GameInput {
private:
    bool keys[SDL_SCANCODE_COUNT] = { false };
    bool mouseButtons[5] = { false }; // Support up to 5 mouse buttons
    glm::ivec2 mouseCoord = glm::ivec2(0);
    int mouseWheel = 0;
public:
    GameInput();
    void Update();
    void ProcessEvent(const SDL_Event& event);
    bool IsKeyPressed(SDL_Scancode key) const;
    bool IsMousePressed(Uint8 button) const;
    glm::ivec2 GetMousePosition() const;
    int GetMouseWheelDelta() const;
};