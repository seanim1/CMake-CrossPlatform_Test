#include "GameInput.h"
#include <iostream>

GameInput::GameInput() {
    SDL_memset(keys, 0, sizeof(keys));
    SDL_memset(mouseButtons, 0, sizeof(mouseButtons));
}

void GameInput::ProcessEvent(const SDL_Event& event) {
    switch (event.type) {
    case SDL_EVENT_KEY_DOWN:
        keys[event.key.scancode] = true;
        SDL_Log("Key pressed: %s", SDL_GetScancodeName(event.key.scancode));
        break;

    case SDL_EVENT_KEY_UP:
        keys[event.key.scancode] = false;
        SDL_Log("Key released: %s", SDL_GetScancodeName(event.key.scancode));
        break;

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (event.button.button < 5) {
            mouseButtons[event.button.button] = true;
        }
        SDL_Log("Mouse button pressed: %d", static_cast<int>(event.button.button));
        break;

    case SDL_EVENT_MOUSE_BUTTON_UP:
        if (event.button.button < 5) {
            mouseButtons[event.button.button] = false;
        }
        SDL_Log("Mouse button released: %d", static_cast<int>(event.button.button));
        break;

    case SDL_EVENT_MOUSE_MOTION:
        mouseCoord = glm::ivec2(event.motion.x, event.motion.y);
        SDL_Log("Mouse Positions: %d, %d", mouseCoord.x, mouseCoord.y);
        break;

    case SDL_EVENT_MOUSE_WHEEL:
        mouseWheel = event.wheel.y;
        SDL_Log("Mouse Wheel: %d", mouseWheel);
        break;
    }
}

void GameInput::Update() {
    mouseWheel = 0; // Reset wheel delta every frame

    // Example of checking if a specific key is held down
    if (keys[SDL_SCANCODE_W]) SDL_Log("Holding W");
    if (keys[SDL_SCANCODE_A]) SDL_Log("Holding A");
    if (keys[SDL_SCANCODE_S]) SDL_Log("Holding S");
    if (keys[SDL_SCANCODE_D]) SDL_Log("Holding D");
}

bool GameInput::IsKeyPressed(SDL_Scancode key) const {
    return keys[key];
}

bool GameInput::IsMousePressed(Uint8 button) const {
    return button < 5 && mouseButtons[button];
}

glm::ivec2 GameInput::GetMousePosition() const {
    return mouseCoord;
}

int GameInput::GetMouseWheelDelta() const {
    return mouseWheel;
}
