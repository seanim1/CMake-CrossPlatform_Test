#pragma once

#include "Global.h"

#ifdef USE_GPU
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#if defined(_WIN32)
#include <windows.h>
#include <vulkan/vulkan_win32.h>
#elif defined(__ANDROID__)
#include <vulkan/vulkan_android.h>
#elif defined(__linux__) && !defined(USING_WAYLAND)
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

#include <vector>

namespace VulkanResourceHelper {
    // Declarations (only prototypes here)
    VkFormat findSupportedFormat(VkPhysicalDevice physicalDevice, const std::vector<VkFormat>& candidates,
        VkImageTiling tiling, VkFormatFeatureFlags features);

    uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

    VkFormat findDepthFormat(VkPhysicalDevice physicalDevice);

    VkImageView createImageView(VkDevice logicalDevice, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

    void createImage(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height,
        VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
        VkImage& image, VkDeviceMemory& imageMemory);

    void createBuffer(VkDevice logicalDevice, VkPhysicalDevice physicalDevice,
        VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
        VkBuffer& buffer, VkDeviceMemory& bufferMemory);

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size,
        VkDevice logicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue);
}