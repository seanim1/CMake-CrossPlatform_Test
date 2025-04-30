#include "VulkanDesc.h"

// Initialize the static variable to 0 (no descriptors created initially)
int VulkanDesc::numDescriptors = 0;
std::vector<VkDescriptorPoolSize> VulkanDesc::descriptorPoolSizes;

uint32_t VulkanDesc::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
    throw std::runtime_error("Failed to find suitable memory type!");
}
