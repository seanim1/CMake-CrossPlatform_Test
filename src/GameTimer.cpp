#include "GameTimer.h"
#include <iostream>

GameTimer::GameTimer() :
    kHardwareTimerFrequency(SDL_GetPerformanceFrequency()),
    kTargetFrametime_HardwareSec(TARGET_FRAMETIME_MICROSECOND * kHardwareTimerFrequency / kSecToMicrosec),
    deltaTime(0), // Stores previous frame's timestamp
    elapsedTime(0) // total time since program start in seconds
{

}

void GameTimer::StartTimer() {
    frameStartTime_HardwareSec = SDL_GetPerformanceCounter();
}

void GameTimer::EndTimer() {
#ifdef LOCK_60FPS
    int64_t frameEndTime_HardwareSec = SDL_GetPerformanceCounter();
    // HardwareSec (assume it will be less than MicroSecond) -> ScaledHardwareSec -> MicroSecond
    int64_t cpuWorkTime_HardwareSec = (frameEndTime_HardwareSec - frameStartTime_HardwareSec);
#if 0
    float cpuWorkTime_Sec = (float)cpuWorkTime_HardwareSec / kHardwareTimerFrequency;
    SDL_Log("cpuWorkTime_Sec: %.8f", cpuWorkTime_Sec);
    SDL_Log("DeltaTime: %.8f", deltaTime);
    SDL_Log("ElapsedTime: %.8f", elapsedTime);
#endif
    // if we are done sooner than 16ms, then do busy waiting.
    while (cpuWorkTime_HardwareSec < kTargetFrametime_HardwareSec) {
        frameEndTime_HardwareSec = SDL_GetPerformanceCounter();
        cpuWorkTime_HardwareSec = (frameEndTime_HardwareSec - frameStartTime_HardwareSec);
        // If we are less than 75% of the way through the current frame, then rest for 1 ms.
        if (cpuWorkTime_HardwareSec < (kTargetFrametime_HardwareSec * (1. - (2. / 16.)))) {
            SDL_Delay(1);
        }
    }
    frameEndTime_HardwareSec = SDL_GetPerformanceCounter();
    cpuWorkTime_HardwareSec = (frameEndTime_HardwareSec - frameStartTime_HardwareSec);
    deltaTime = (float)cpuWorkTime_HardwareSec / kHardwareTimerFrequency;
    elapsedTime += deltaTime;
#endif
}
