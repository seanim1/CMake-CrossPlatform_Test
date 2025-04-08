#pragma once

#include "GameScreen.h"

//class GameScreen {
//public:
//    SDL_Texture* texture;
//    glm::ivec2 dimension;
//
//    GameScreen(int width, int height)
//        : dimension(width, height) {
//    }
//
//    ~GameScreen() {
//
//    }
//};

GameScreen::GameScreen(int width, int height, SDL_Renderer* renderer)
    : dimension(width, height), image(width * height) {
    // Create a texture for the pixel buffer (RGBA format)
    //SDL_Texture* texture = SDL_CreateTexture(renderer,
    //    SDL_PIXELFORMAT_RGBA32,   // SDL3 uses SDL_PIXELFORMAT_RGBA32
    //    SDL_TEXTUREACCESS_STREAMING, /**< Changes frequently, lockable */
    //    dimension.x, dimension.y);
#ifdef USE_GPU
#else
    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,  // Format that matches glm::u8vec4
        SDL_TEXTUREACCESS_STREAMING,
        width, height
    );
    if (texture == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateTexture Error: %s", SDL_GetError());
    }
#endif
}
GameScreen::~GameScreen() {
    SDL_DestroyTexture(texture);
}
void GameScreen::Update(float elapsedTime, SDL_Renderer* renderer) {
    Draw(elapsedTime);
    // Update texture with pixel data
    SDL_UpdateTexture(texture,
        NULL,   // Update entire texture
        image.data(),  // Pixel data
        dimension.x * sizeof(glm::u8vec4)   // Pitch (bytes per row)
    );
    //SDL_RenderClear(renderer); // Clears the entire rendering target (the backbuffer). Ensures that previous frames' content doesn't persist.
    SDL_RenderTexture(renderer, texture, NULL, NULL); // Renders the texture to the screen
    SDL_RenderPresent(renderer); // Swaps the back buffer with the front buffer
}
void GameScreen::Draw(float elapsedTime) {
    // Fill with example data - a simple gradient
    for (int y_Screen = 0; y_Screen < dimension.y; y_Screen++) {
        int rowIdx_Screen = y_Screen * dimension.x;
        float y_Normalized = (float)y_Screen / dimension.y;
        for (int x_Screen = 0; x_Screen < dimension.x; x_Screen++) {
            float x_Normalized = (float)x_Screen / dimension.x;
            glm::vec3 col = 0.5f + 0.5f * glm::cos(elapsedTime + glm::vec3(x_Normalized, y_Normalized, x_Normalized) + glm::vec3(0, 2, 4));

            image[y_Screen * dimension.x + x_Screen] = glm::u8vec4(col * 255.f, 255);
        }
    }
}