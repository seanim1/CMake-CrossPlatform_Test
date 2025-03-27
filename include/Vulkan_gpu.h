#pragma once
#include <vector>
#include "Global.h"
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#if defined(_WIN32)
#include <windows.h>
#include <vulkan/vulkan_win32.h>
#elif defined(VK_USE_PLATFORM_XCB_KHR)
#include <vulkan/vulkan_xcb.h>
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
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
void initVulkan(GLFWwindow* window);

