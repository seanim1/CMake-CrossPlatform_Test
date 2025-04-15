#include "VulkanQueue.h"

VulkanQueue::VulkanQueue(uint32_t queueFamilyIndex, uint32_t queueCount)
	: queueFamilyIndex(queueFamilyIndex)
{
}
void VulkanQueue::initQueue(VkDevice logicalDevice) {
	// logicalDevice
	vkGetDeviceQueue(logicalDevice, queueFamilyIndex, 0, &queue);
}