#include "Global.h"
#include "GameWindow.h"
#include "GameScreen.h"
#include "GameInput.h"
#include "GameTimer.h"

#include "Vulkan_gpu.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <glm/glm.hpp>

//#define USE_GPU

int main() {
    GameWindow* gWindow = new GameWindow(800, 600, "Cross-Platform GUI");
    GameScreen* gScreen = new GameScreen(80, 60, gWindow->renderer);
    GameInput* gInput = new GameInput();
    GameTimer* gTimer = new GameTimer();

#ifdef USE_GPU
    initVulkan(gWindow->window);
#endif
    bool running = true;

    while (running) {
        gTimer->StartTimer();
        gInput->Update();
#ifdef USE_GPU
#else
        gScreen->Update(gTimer->elapsedTime, gWindow->renderer);
#endif
        gTimer->EndTimer();
    }
    delete gTimer;
    delete gInput;
    delete gScreen;
    delete gWindow;
    return 0;
}
