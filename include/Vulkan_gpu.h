#pragma once
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
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
#include <vulkan/vulkan_metal.h>
#endif

#define GAME_NAME "Cross_Platform_Demo"
#ifdef _DEBUG
#define VALIDATION_LAYER_VULKAN 1
#define PRESENT_IMG_COUNT 2
#else
#define VALIDATION_LAYER_VULKAN 0
#endif
void initVulkan(GLFWwindow* window);

