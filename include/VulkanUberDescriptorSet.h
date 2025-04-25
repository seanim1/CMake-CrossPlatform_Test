#pragma once
#include "Global.h"
#include <SDL3/SDL.h>
#include "VulkanDescBuffer.h"

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

class VulkanUberDescriptorSet {
private:

public:
	VulkanUberDescriptorSet::VulkanUberDescriptorSet(VkDevice logicalDevice, std::vector<VulkanDesc*>& descriptorList);
	VkDescriptorSetLayout uberSetLayout;
	VkDescriptorPool descPool;
	VkDescriptorSet uberDescSet;
	/* information about where to expect resource bindings, used for Pipeline creation */
	VkPipelineLayout uberPipelineLayout;
};