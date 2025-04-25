#include "VulkanPhysicalDevice.h"

VulkanPhysicalDevice::VulkanPhysicalDevice(VkInstance instance)
{
	uint32_t gpuCount = 0;
	vkEnumeratePhysicalDevices(instance, &gpuCount, NULL);
	SDL_Log("vkEnumeratePhysicalDevices: GPU count: %d\n", gpuCount);
	// Enumerate devices
	gpuCount = 1; // use the very first GPU, GPU count should be set to 1.
	vkEnumeratePhysicalDevices(instance, &gpuCount, &physicalDevice);
	vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
	SDL_Log("Physical Device & QueueFamily:");
	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		SDL_Log("\t deviceType: DISCRETE_GPU");
	}
	else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
		SDL_Log("\t deviceType: INTEGRATED_GPU");
	}
	else {
		SDL_Log("\t deviceType: other");
	}

	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemoryProperties);


	// physical device
	VkQueueFamilyProperties* queueFamilyProperties;
	uint32_t queueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, NULL);
	//if (queueFamilyCount <= 0) { ErrorWindow_EVENT(FILE_AND_LINE); }
	queueFamilyProperties = (VkQueueFamilyProperties*)malloc(queueFamilyCount * sizeof(VkQueueFamilyProperties));
	//if (queueFamilyProperties == NULL) { ErrorWindow_EVENT(FILE_AND_LINE); }
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties);
	SDL_Log("\t Physical Device Queue Family (%d count):", queueFamilyCount);
	for (uint32_t i = 0; i < queueFamilyCount; i++) {
		SDL_Log("\t\t %d: VkQueueFlags: %d, QueueCount: %d", i, queueFamilyProperties[i].queueFlags, queueFamilyProperties[i].queueCount);
	}
	computeQueueFamilyIndex = 0;
	for (uint32_t j = 0; j < queueFamilyCount; j++) {
		if (1) printf("\t Queue Family Index: %d\n", computeQueueFamilyIndex);
		break;
	}
}
