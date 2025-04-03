#include "GameInput.h"
#include <iostream>

GameInput::GameInput() {
    SDL_memset(keys, 0, sizeof(keys));
    SDL_memset(mouseButtons, 0, sizeof(mouseButtons));
}

void GameInput::Update() {
    SDL_Event event;
    mouseWheel = 0; // Reset wheel delta every frame

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_EVENT_QUIT:
            std::cout << "Quit event detected!\n";
            break;

        case SDL_EVENT_KEY_DOWN:
            keys[event.key.scancode] = true;
            std::cout << "Key pressed: " << SDL_GetScancodeName(event.key.scancode) << std::endl;
            break;

        case SDL_EVENT_KEY_UP:
            keys[event.key.scancode] = false;
            std::cout << "Key released: " << SDL_GetScancodeName(event.key.scancode) << std::endl;
            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if (event.button.button < 5) {
                mouseButtons[event.button.button] = true;
            }
            std::cout << "Mouse button pressed: " << (int)event.button.button << std::endl;
            break;

        case SDL_EVENT_MOUSE_BUTTON_UP:
            if (event.button.button < 5) {
                mouseButtons[event.button.button] = false;
            }
            std::cout << "Mouse button released: " << (int)event.button.button << std::endl;
            break;

        case SDL_EVENT_MOUSE_MOTION:
            mouseCoord = glm::ivec2(event.motion.x, event.motion.y);
            std::cout << "Mouse Postions: " << mouseCoord.x << ", " << mouseCoord.y << std::endl;
            break;

        case SDL_EVENT_MOUSE_WHEEL:
            mouseWheel = event.wheel.y; // Positive = up, Negative = down
            std::cout << "Mouse Wheel: " << mouseWheel << std::endl;
            break;
        }
    }

    // Example of checking if a specific key is held down
    if (keys[SDL_SCANCODE_W]) std::cout << "Holding W" << std::endl;
    if (keys[SDL_SCANCODE_A]) std::cout << "Holding A" << std::endl;
    if (keys[SDL_SCANCODE_S]) std::cout << "Holding S" << std::endl;
    if (keys[SDL_SCANCODE_D]) std::cout << "Holding D" << std::endl;
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
