#pragma once
#include "Global.h"
#include "GeometryBase.h"
#include <vector>
#include <fstream>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>

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
typedef struct SpecializationConstantData {
	int Screen_Width;
	int Screen_Heigt;
} SpecializationConstantData;
class VulkanGraphicsPipeline {
private:
	VkShaderModule VulkanGraphicsPipeline::createShaderModule(const std::vector<char>& code, VkDevice logicalDevice);
	std::vector<char> VulkanGraphicsPipeline::readFile(const std::string& filename);
	void initSpecializationConstant();
public:
	VulkanGraphicsPipeline::VulkanGraphicsPipeline(VkDevice logicalDevice, 
		VkExtent2D swapChainExtent, VkSurfaceFormatKHR selectedSurfaceFormat, 
		VkPipelineLayout uberPipelineLayout, Geometry* geometry);
	VkPipeline graphicsPipeline;
	static constexpr int specialization_constant_count = (sizeof(SpecializationConstantData) / 4);  // static, same across all instances
	VkSpecializationMapEntry specializationMapEntries[specialization_constant_count];
	VkSpecializationInfo specializationInfo;
	SpecializationConstantData gpuConstantData;
};