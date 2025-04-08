#define SDL_MAIN_USE_CALLBACKS 1

#include "Global.h"
#include "GameWindow.h"
#include "GameScreen.h"
#include "GameInput.h"
#include "GameTimer.h"
#include "Vulkan_gpu.h"

#include <glm/glm.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_main_impl.h>
#include <SDL3/SDL_vulkan.h>
#include <SDL3/SDL_video.h>

static GameWindow* gWindow = nullptr;
static GameScreen* gScreen = nullptr;
static GameInput* gInput = nullptr;
static GameTimer* gTimer = nullptr;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    gWindow = new GameWindow(1280, 720, "Cross-Platform GUI");
    gScreen = new GameScreen(gWindow->dimension.x / 10, gWindow->dimension.y / 10, gWindow->renderer);
    gInput = new GameInput();
    gTimer = new GameTimer();

#ifdef USE_GPU
    initVulkan(gWindow->window);
#endif

    return SDL_APP_CONTINUE; // SDL_APP_FAILURE to indicate failure
}

/* This function runs when SDL receives input or OS-level events. */
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS; // Clean exit
    }
    gInput->ProcessEvent(*event);
    return SDL_APP_CONTINUE;
}

/* This function runs once per frame. */
SDL_AppResult SDL_AppIterate(void* appstate) {
    gTimer->StartTimer();
    gInput->Update();

#ifdef USE_GPU
    // Vulkan rendering goes here
#else
    gScreen->Update(gTimer->elapsedTime, gWindow->renderer);
#endif

    gTimer->EndTimer();
    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    delete gTimer;
    delete gInput;
    delete gScreen;
    delete gWindow;
}
