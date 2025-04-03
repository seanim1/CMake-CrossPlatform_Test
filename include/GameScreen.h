#pragma once
#include "Global.h"

#include <vector>

#include <glm/glm.hpp>
#include <SDL3/SDL.h>
#include <stdexcept>

class GameScreen {
public:
    glm::ivec2 dimension;
    SDL_Texture* texture;
    std::vector<glm::u8vec4> image;
    GameScreen(int width, int height, SDL_Renderer* renderer);
    ~GameScreen();

    void Update(float elapsedTime, SDL_Renderer* renderer);
private:
    void Draw(float elapsedTime);
};
