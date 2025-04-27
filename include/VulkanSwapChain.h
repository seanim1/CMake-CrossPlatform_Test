#pragma once
#include "Global.h"
#include <vector>
#include <SDL3/SDL.h>

#ifdef USE_GPU
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#if defined(_WIN32)
#include <windows.h>
#include <vulkan/vulkan_win32.h>
#elif defined(__ANDROID__)
#include <vulkan/vulkan_android.h>
#elif defined(__linux__)
#include <xcb/xcb.h>
#include <vulkan/vulkan_xcb.h>
#elif defined(__linux__) && defined(USING_WAYLAND)
#include <wayland-client.h>
#include <vulkan/vulkan_wayland.h>
#elif defined(__APPLE__) // For both MacOS and iOS
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan_metal.h>
#include <vulkan/vulkan_beta.h>
#endif
#endif

#define PRESENT_IMG_COUNT 2
class VulkanSwapChain {
private:

public:
	VulkanSwapChain(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkDevice logicalDevice);
	VkSwapchainKHR swapChain; // one
	VkImage swapChainImages[PRESENT_IMG_COUNT];
	VkImageView swapChainImageView[PRESENT_IMG_COUNT];
	VkSurfaceFormatKHR selectedSurfaceFormat;
	VkExtent2D swapChainExtent;
};
