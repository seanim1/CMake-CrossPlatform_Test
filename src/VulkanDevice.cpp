#include "VulkanDevice.h"

VulkanDevice::VulkanDevice(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, std::vector<const char*>* requestedDeviceExtensions)
{
	SDL_Log("Logical Device and Queue:");
	// Get list of supported GPU extensions
	uint32_t extCount = 0;
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extCount, nullptr); // after a driver update, changed from 221 to 223, must be dynamically allocated
	//if (extCount <= 0) { ErrorWindow_EVENT(FILE_AND_LINE); }
	std::vector<VkExtensionProperties> supportedDeviceExtensions(extCount);
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extCount, supportedDeviceExtensions.data());
	//if (supportedDeviceExtensions == NULL) { ErrorWindow_EVENT(FILE_AND_LINE); }
	// Filter out unsupported extensions
	std::vector<const char*> enabledDeviceExtensions;
	for (const char* extension : *requestedDeviceExtensions) {
		bool found = false;
		for (const auto& supportedExtension : supportedDeviceExtensions) {
			if (strcmp(extension, supportedExtension.extensionName) == 0) {
				enabledDeviceExtensions.push_back(extension);
				std::cout << "\t Device Extension '" << extension << "' is supported.\n";
				found = true;
				break;
			}
		}
		if (!found) {
			std::cout << "\t Device Extension '" << extension << "' is NOT supported.\n";
		}
	}
	VkDeviceCreateInfo deviceCreateInfo; memset(&deviceCreateInfo, 0, sizeof(VkDeviceCreateInfo));
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	// If the physicalDevice has SwapChain extension, it implies that the Device will be used for presentation
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledDeviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = enabledDeviceExtensions.data();

	// I am going to request Graphics and Compute queues
	// Just use one Graphics/compute queue for now
	float queuePriority = 1.0; // Vulkan lets you assign priorities to queues to influence the scheduling of command buffer execution using floating point numbers between 0.0 and 1.0. This is required even if there is only a single queue
	VkDeviceQueueCreateInfo queueCreateInfos; memset(&queueCreateInfos, 0, sizeof(VkDeviceQueueCreateInfo));
	queueCreateInfos.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfos.queueFamilyIndex = queueFamilyIndex;
	queueCreateInfos.queueCount = 1;
	queueCreateInfos.pQueuePriorities = &queuePriority;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pQueueCreateInfos = &queueCreateInfos;

	// >> Enable the synchronization2 feature
	VkPhysicalDeviceSynchronization2FeaturesKHR synchronization2Features; memset(&synchronization2Features, 0, sizeof(VkPhysicalDeviceSynchronization2FeaturesKHR));
	synchronization2Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR;
	// >> Enable the Buffer Device Address feature
	// Step 1: Initialize the VkPhysicalDeviceBufferDeviceAddressFeatures structure
	VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures; memset(&bufferDeviceAddressFeatures, 0, sizeof(VkPhysicalDeviceBufferDeviceAddressFeatures));
	bufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
	// >> Enable the 16BitStorage feature
	VkPhysicalDevice16BitStorageFeaturesKHR storage16bitFeatures; memset(&storage16bitFeatures, 0, sizeof(VkPhysicalDevice16BitStorageFeaturesKHR));
	storage16bitFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES_KHR;
	// >> Enable the 8BitStorage feature
	VkPhysicalDevice8BitStorageFeaturesKHR storage8bitFeatures; memset(&storage8bitFeatures, 0, sizeof(VkPhysicalDevice8BitStorageFeaturesKHR));
	storage8bitFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR;

	// >> Enable the Float16Int8 feature
	VkPhysicalDeviceShaderFloat16Int8FeaturesKHR float16Int8Features; memset(&float16Int8Features, 0, sizeof(VkPhysicalDeviceShaderFloat16Int8FeaturesKHR));
	float16Int8Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR;

	// >> Enable the Dynamic Rendering feature
	VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures{};
	dynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
	//dynamicRenderingFeatures.dynamicRendering = VK_TRUE;

	VkPhysicalDeviceShaderAtomicFloatFeaturesEXT atomicFloatFeatures; memset(&atomicFloatFeatures, 0, sizeof(VkPhysicalDeviceShaderAtomicFloatFeaturesEXT));
	atomicFloatFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT;

	VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT atomicFloat2Features; memset(&atomicFloat2Features, 0, sizeof(VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT));
	atomicFloat2Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_2_FEATURES_EXT;

	//VkPhysicalDeviceShaderAtomicInt64FeaturesKHR atomicInt64Features; memset(&atomicInt64Features, 0, sizeof(VkPhysicalDeviceShaderAtomicInt64FeaturesKHR));
	//atomicInt64Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_INT64_FEATURES_KHR;
	//
	//VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT imageAtomicInt64Features; memset(&imageAtomicInt64Features, 0, sizeof(VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT));
	//imageAtomicInt64Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_ATOMIC_INT64_FEATURES_EXT;

	// Step 2: Initialize the VkPhysicalDeviceFeatures2 structure
	VkPhysicalDeviceFeatures2 deviceFeatures2; memset(&deviceFeatures2, 0, sizeof(VkPhysicalDeviceFeatures2));
	deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

	// Chain the structures
	deviceFeatures2.pNext = &bufferDeviceAddressFeatures;
	bufferDeviceAddressFeatures.pNext = &synchronization2Features;
	synchronization2Features.pNext = &storage8bitFeatures;
	storage8bitFeatures.pNext = &storage16bitFeatures;
	storage16bitFeatures.pNext = &float16Int8Features;
	float16Int8Features.pNext = &dynamicRenderingFeatures;
	dynamicRenderingFeatures.pNext = &atomicFloatFeatures;
	//atomicFloatFeatures.pNext = &atomicInt64Features;
	//atomicInt64Features.pNext = &imageAtomicInt64Features;

	// Step 3: Query the physical device for supported features
	vkGetPhysicalDeviceFeatures2(physicalDevice, &deviceFeatures2);

	printf("\t VkPhysicalDeviceFeatures2 Queries:\n");
	// Step 4: Check if bufferDeviceAddressCaptureReplay is supported
	if (bufferDeviceAddressFeatures.bufferDeviceAddressCaptureReplay) {
		printf("\t\t bufferDeviceAddressCaptureReplay is supported.\n");
	}
	else {
		printf("\t\t bufferDeviceAddressCaptureReplay is NOT supported.\n");
	}
	if (synchronization2Features.synchronization2) {
		printf("\t\t synchronization2 is supported.\n");
	}
	else {
		printf("\t\t synchronization2 is NOT supported.\n");
	}
	if (synchronization2Features.synchronization2) {
		deviceCreateInfo.pNext = &deviceFeatures2;
	}
	if (storage8bitFeatures.uniformAndStorageBuffer8BitAccess && storage16bitFeatures.storageBuffer16BitAccess) {
		printf("\t\t 8-bit and 16-bit storage features are supported.\n");
	}
	else {
		printf("\t\t 8-bit or 16-bit storage features are not supported.\n");
	}
	if (atomicFloatFeatures.shaderImageFloat32AtomicAdd && atomicFloatFeatures.shaderImageFloat32Atomics) {
		printf("\t\t shaderImageFloat32AtomicAdd is supported.\n");
	}
	else {
		printf("\t\t shaderImageFloat32AtomicAdd is not supported.\n");
	}
	if (atomicFloat2Features.shaderBufferFloat16AtomicAdd) {
		printf("\t\t shaderBufferFloat16AtomicAdd is supported.\n");
	}
	else {
		printf("\t\t shaderBufferFloat16AtomicAdd is not supported.\n");
	}
	if (dynamicRenderingFeatures.dynamicRendering) {
		printf("\t\t dynamicRendering is supported.\n");
	}
	else {
		printf("\t\t dynamicRendering is not supported.\n");
	}
	VkResult result = vkCreateDevice(physicalDevice, &deviceCreateInfo, NULL, &logicalDevice);
	if (result != VK_SUCCESS) {
		printf("Failed to create Vulkan Device! Error code: %d\n", result);
	}
}
