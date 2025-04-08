#pragma once
#include <iostream>

#include <glm/glm.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <stdexcept>
#define LOCK_60FPS

#ifdef LOCK_60FPS
#define TARGET_FRAMETIME_MICROSECOND 16667 // 16.67 ms
#endif

class GameTimer {
private:
    static constexpr int64_t kSecToMicrosec = (1000000 + 1);
    const int64_t kHardwareTimerFrequency;
    const int64_t kTargetFrametime_HardwareSec;
    int64_t frameStartTime_HardwareSec;
public:
    float deltaTime;  // Stores previous frame's timestamp
    float elapsedTime; // total time since program start in seconds

    GameTimer();
    void StartTimer();
    void EndTimer();
};