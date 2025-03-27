#pragma once
#include "Global.h"
#include <vulkan/vulkan.h>
#include <windows.h>
#include <vulkan/vulkan_win32.h>
#include <GLFW/glfw3.h>

#define GAME_NAME "Cross_Platform_Demo"
#ifdef _DEBUG
#define VALIDATION_LAYER_VULKAN 1
#define PRESENT_IMG_COUNT 2
#else
#define VALIDATION_LAYER_VULKAN 0
#endif
void initVulkan(GLFWwindow* window);

