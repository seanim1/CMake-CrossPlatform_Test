#pragma once
#include <vector>
#include "Global.h"
#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#if defined(_WIN32)
#include <windows.h>
#include <vulkan/vulkan_win32.h>
#elif defined(__linux__)
#include <xcb/xcb.h>
#include <vulkan/vulkan_xcb.h>
#elif defined(__linux__) && defined(USING_WAYLAND)
#include <wayland-client.h>
#include <vulkan/vulkan_wayland.h>
#elif defined(__APPLE__)
#include <vulkan/vulkan_metal.h>
#endif
#define GAME_NAME "Cross_Platform_Demo"
#define PRESENT_IMG_COUNT 2
#ifdef _DEBUG
#define VALIDATION_LAYER_VULKAN 1
#else
#define VALIDATION_LAYER_VULKAN 0
#endif
void initVulkan(SDL_Window* window);

