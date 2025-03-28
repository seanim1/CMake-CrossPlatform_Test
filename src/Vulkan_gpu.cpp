#include "Vulkan_gpu.h"
static VkInstance instance;
static VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
static VkPhysicalDevice physicalDevice;
static VkPhysicalDeviceProperties deviceProperties;
static uint32_t computeQueueFamilyIndex;
static VkDevice logicalDevice;
static VkQueue queue;
static VkSurfaceKHR surface; // one
static VkSwapchainKHR swapChain; // one
static VkImage swapChainImages[PRESENT_IMG_COUNT];
static VkImageView swapChainImageView[PRESENT_IMG_COUNT];
static VkSurfaceFormatKHR selectedSurfaceFormat;

static void createInstance() {
	printf("Vulkan Instance:\n");
	VkApplicationInfo appInfo; memset(&appInfo, 0, sizeof(VkApplicationInfo));
	VkApplicationInfo* appInfoP = &appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = GAME_NAME;
	appInfo.pEngineName = GAME_NAME "_Engine";
	appInfo.apiVersion = VK_API_VERSION_1_3;

	uint32_t extCount = 0;
	vkEnumerateInstanceExtensionProperties(NULL, &extCount, NULL);

	std::vector<VkExtensionProperties> supportedInstanceExtensions(extCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extCount, supportedInstanceExtensions.data());
	// if (supportedInstanceExtensions == NULL) { (FILE_AND_LINE); }
	std::cout << "Supported Instance Extensions (" << extCount << " count):\n";
	for (uint32_t i = 0; i < extCount; i++) {
		std::cout << "\t" << i << ": " << supportedInstanceExtensions[i].extensionName << "\n";
	}
	std::vector<const char*> requestingInstanceExtensions = {
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
		VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME,
	};
#if defined(_WIN32)
	requestingInstanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME); // vkCreateWin32SurfaceKHR() will fail without it
#elif defined(__APPLE__)
	requestingInstanceExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME); // vkCreateInstance will fail with VK_ERROR_INCOMPATIBLE_DRIVER: https://stackoverflow.com/questions/72789012/why-does-vkcreateinstance-return-vk-error-incompatible-driver-on-macos-despite
	requestingInstanceExtensions.push_back(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
#elif defined(__linux__)
	requestingInstanceExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(__linux__) && defined(USING_WAYLAND)
	requestingInstanceExtensions.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#endif

	// Filter out unsupported extensions
	std::vector<const char*> enabledInstanceExtensions;
	for (const char* extension : requestingInstanceExtensions) {
		bool found = false;
		for (const auto& supportedExtension : supportedInstanceExtensions) {
			if (strcmp(extension, supportedExtension.extensionName) == 0) {
				enabledInstanceExtensions.push_back(extension);
				std::cout << "\t Instance Extension '" << extension << "' is supported.\n";
				found = true;
				break;
			}
		}
		if (!found) {
			std::cout << "\t Instance Extension '" << extension << "' is NOT supported.\n";
		}
	}
	VkInstanceCreateInfo instanceCreateInfo; memset(&instanceCreateInfo, 0, sizeof(instanceCreateInfo));
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pNext = NULL;
#if defined(__APPLE__)
    // Enable portability enumeration bit
    instanceCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif
	instanceCreateInfo.pApplicationInfo = (VkApplicationInfo*)&appInfo;
	instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledInstanceExtensions.size());
	instanceCreateInfo.ppEnabledExtensionNames = enabledInstanceExtensions.data();
	const char* validationLayerName = "VK_LAYER_KHRONOS_validation";
	instanceCreateInfo.ppEnabledLayerNames = (const char* const*)&validationLayerName;
	instanceCreateInfo.enabledLayerCount = 1;
	if (VALIDATION_LAYER_VULKAN) { // validation layer for Vulkan debugging
		instanceCreateInfo.enabledLayerCount = 1;
	}
    VkResult result = vkCreateInstance(&instanceCreateInfo, NULL, &instance);
    if (result != VK_SUCCESS) {
        printf("Failed to create Vulkan instance! Error code: %d\n", result);
    }
    //VkResult resulteif = vkCreateInstance(&instanceCreateInfo, NULL, &instance);
	//printf("vkCreateInstance %d.\n", resulteif);
	uint32_t gpuCount = 0;
	vkEnumeratePhysicalDevices(instance, &gpuCount, NULL);
}

static void queryPhysicalDevice_selectQueueFamily() {
	uint32_t gpuCount = 0;
	(vkEnumeratePhysicalDevices(instance, &gpuCount, NULL));
	// Enumerate devices
	gpuCount = 1; // use the very first GPU, GPU count should be set to 1.
	vkEnumeratePhysicalDevices(instance, &gpuCount, &physicalDevice);
	vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
	printf("Physical Device & QueueFamily:\n");
	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		printf("\t deviceType: DISCRETE_GPU\n");
	}
	else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
		printf("\t deviceType: INTEGRATED_GPU\n");
	}
	else {
		printf("\t deviceType: other\n");
	}

	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemoryProperties);
	VkQueueFamilyProperties* queueFamilyProperties;
	uint32_t queueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, NULL);
	//if (queueFamilyCount <= 0) { ErrorWindow_EVENT(FILE_AND_LINE); }
	queueFamilyProperties = (VkQueueFamilyProperties*)malloc(queueFamilyCount * sizeof(VkQueueFamilyProperties));
	//if (queueFamilyProperties == NULL) { ErrorWindow_EVENT(FILE_AND_LINE); }
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties);
	printf("\t Physical Device Queue Family (%d count):\n", queueFamilyCount);
	for (uint32_t i = 0; i < queueFamilyCount; i++) {
		printf("\t\t %d: VkQueueFlags: %d, QueueCount: %d\n", i, queueFamilyProperties[i].queueFlags, queueFamilyProperties[i].queueCount);
	}
}

static void queryExtensions_createLogicalDevice_getQueue() {
	printf("Logical Device and Queue:\n");
	// Get list of supported GPU extensions
	uint32_t extCount = 0;
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extCount, nullptr); // after a driver update, changed from 221 to 223, must be dynamically allocated
	//if (extCount <= 0) { ErrorWindow_EVENT(FILE_AND_LINE); }
	std::vector<VkExtensionProperties> supportedDeviceExtensions(extCount);
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extCount, supportedDeviceExtensions.data());
	//if (supportedDeviceExtensions == NULL) { ErrorWindow_EVENT(FILE_AND_LINE); }
	std::vector<const char*> requestingDeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_DEVICE_GROUP_EXTENSION_NAME,
		VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
		VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME, // for VK_ACCESS_NONE in the pipeline barrier
		VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME,  // for GL_EXT_shader_explicit_arithmetic_types_float16
		VK_KHR_16BIT_STORAGE_EXTENSION_NAME,
		VK_KHR_8BIT_STORAGE_EXTENSION_NAME,
		VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_EXTENSION_NAME,
		VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME, // for shader printf: GL_EXT_debug_printf
		VK_KHR_SHADER_ATOMIC_INT64_EXTENSION_NAME,
		VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME, // not fully supported on RX6600 (imageAtomicAdd not supported)
		//VK_EXT_SHADER_IMAGE_ATOMIC_INT64_EXTENSION_NAME,
	};
#if defined(__APPLE__)
	// From SaschaWillems - When running on iOS/macOS with MoltenVK and VK_KHR_portability_subset is defined and supported by the device, enable the extension
    requestingDeviceExtensions.push_back(VK_KHR_portability_subset);

#endif
	// Filter out unsupported extensions
	std::vector<const char*> enabledDeviceExtensions;
	for (const char* extension : requestingDeviceExtensions) {
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
	queueCreateInfos.queueFamilyIndex = computeQueueFamilyIndex;
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
	storage16bitFeatures.storageBuffer16BitAccess = VK_TRUE;
	// >> Enable the 8BitStorage feature
	VkPhysicalDevice8BitStorageFeaturesKHR storage8bitFeatures; memset(&storage8bitFeatures, 0, sizeof(VkPhysicalDevice8BitStorageFeaturesKHR));
	storage8bitFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR;
	storage8bitFeatures.uniformAndStorageBuffer8BitAccess = VK_TRUE;

	// >> Enable the Float16Int8 feature
	VkPhysicalDeviceShaderFloat16Int8FeaturesKHR float16Int8Features; memset(&float16Int8Features, 0, sizeof(VkPhysicalDeviceShaderFloat16Int8FeaturesKHR));
	float16Int8Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR;
	float16Int8Features.shaderFloat16 = VK_TRUE; // Enable Float16 support
	float16Int8Features.shaderInt8 = VK_TRUE;    // Enable Int8 support

	VkPhysicalDeviceShaderAtomicFloatFeaturesEXT atomicFloatFeatures; memset(&atomicFloatFeatures, 0, sizeof(VkPhysicalDeviceShaderAtomicFloatFeaturesEXT));
	atomicFloatFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT;
	atomicFloatFeatures.shaderBufferFloat32Atomics = VK_TRUE;
	atomicFloatFeatures.shaderBufferFloat32AtomicAdd = VK_TRUE;
	atomicFloatFeatures.shaderImageFloat32Atomics = VK_TRUE;
	atomicFloatFeatures.shaderImageFloat32AtomicAdd = VK_TRUE;

	VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT atomicFloat2Features; memset(&atomicFloat2Features, 0, sizeof(VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT));
	atomicFloat2Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_2_FEATURES_EXT;
	atomicFloat2Features.shaderSharedFloat16AtomicAdd = VK_TRUE;

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
	float16Int8Features.pNext = &atomicFloatFeatures;
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
    VkResult result = vkCreateDevice(physicalDevice, &deviceCreateInfo, NULL, &logicalDevice);
    if (result != VK_SUCCESS) {
        printf("Failed to create Vulkan Device! Error code: %d\n", result);
    }

	vkGetDeviceQueue(logicalDevice, computeQueueFamilyIndex, 0, &queue);
}

static void createSurface(GLFWwindow* window) {
	// Create the os-specific surface
//#if defined(_WIN32)
//	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo; memset(&surfaceCreateInfo, 0, sizeof(VkWin32SurfaceCreateInfoKHR));
//	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
//	surfaceCreateInfo.hinstance = GetModuleHandle(NULL);
//	surfaceCreateInfo.hwnd = Handle_to_Window;
//	(vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, NULL, &surface));
//#elif defined(VK_USE_PLATFORM_MACOS_MVK)
//	requestingInstanceExtensions[1] = VK_MVK_MACOS_SURFACE_EXTENSION_NAME;
//#elif defined(VK_USE_PLATFORM_XCB_KHR)
//	requestingInstanceExtensions[1] = VK_KHR_XCB_SURFACE_EXTENSION_NAME;
//#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
//	requestingInstanceExtensions[1] = VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME;
//#endif
	VkResult result = glfwCreateWindowSurface(instance, window, nullptr, &surface); // Vulkan GLFW
	if (result != VK_SUCCESS) {
        printf("Failed to create Vulkan Surface! Error code: %d\n", result);
    }
}
static void createSwapChain_Images_ImageViews() {
	//	1. Get physical device surface properties and formats: Surface Capabilities (min_C/max_C number of images in swap chain, min_C/max_C width and height of images)
	VkSurfaceCapabilitiesKHR surfCaps;
	(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfCaps));
	VkExtent2D swapchainExtent = surfCaps.currentExtent; // Swapchain image size = Extent
	// Determine the number of images
	uint32_t swapchainImageCount = surfCaps.minImageCount;
	if ((surfCaps.maxImageCount > 0) && (swapchainImageCount > surfCaps.maxImageCount)) {
		swapchainImageCount = surfCaps.maxImageCount;
	}
	//	2. Select a Surface Format (pixel format, color space)
	uint32_t formatCount;
	(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, NULL));
	VkSurfaceFormatKHR* surfaceFormats;
	surfaceFormats = (VkSurfaceFormatKHR*)malloc(formatCount * sizeof(VkSurfaceFormatKHR));
	(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats));
	printf("Supported Physical Device Surface Formats (%d count):\n", formatCount);
	for (int i = 0; i < formatCount; i++) {
		printf("\t%d: Format: %d, ColorSpace: %d\n", i, surfaceFormats[i].format, surfaceFormats[i].colorSpace);
	}
	// We want to get a format that best suits our needs, but the first format is okay if they aren't available
	VkFormat desiredSurfaceImageFormats[] = {
		VK_FORMAT_B8G8R8A8_UNORM,
		VK_FORMAT_R8G8B8A8_UNORM,
		VK_FORMAT_A8B8G8R8_UNORM_PACK32
	};
	selectedSurfaceFormat = surfaceFormats[0];
	uint8_t foundDesiredSurfaceImageFormat = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < formatCount; j++) {
			if (desiredSurfaceImageFormats[i] == surfaceFormats[j].format) {
				selectedSurfaceFormat = surfaceFormats[j];
				foundDesiredSurfaceImageFormat = i;
				break;
			}
		}
		if (foundDesiredSurfaceImageFormat) break;
	}
	// Switch-statement is faster than if-statement via Jump Table, make sure cases are contiguous in value (0, 1, 2...)
	switch (foundDesiredSurfaceImageFormat) {
	case 0:
		printf("\tSelected Surface Image Format: VK_FORMAT_B8G8R8A8_UNORM\n");
		break;
	case 1:
		printf("\tSelected Surface Image Format: VK_FORMAT_R8G8B8A8_UNORM\n");
		break;
	case 2:
		printf("\tSelected Surface Image Format: VK_FORMAT_A8B8G8R8_UNORM_PACK32\n");
		break;
	}
	free(surfaceFormats);
	//	3. Select a present mode for the swapchain. Available presentation modes (Immediate, Mailbox,..)
	uint32_t presentModeCount;
	(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, NULL));
	VkPresentModeKHR* presentModes;
	presentModes = (VkPresentModeKHR*)malloc(presentModeCount * sizeof(VkPresentModeKHR));
	//if (presentModes == NULL) { ErrorWindow_EVENT(FILE_AND_LINE); }
	(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes));
	VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR; // This is the only presentMode that is guaranteed to be present. FIFO won't tear the screen, giving my game the most polished look.
	//for (uint32_t i = 0; i < presentModeCount; i++) {
	//	if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
	//		swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR; // Mailbox is preferred for PC, FIFO is preferred for energy usage
	//		break;
	//	}
	//	if (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
	//}
	free(presentModes);
	// alpha compositing mode to use when this surface is composited together with other surfaces on certain window systems
	VkCompositeAlphaFlagBitsKHR alphaCompositeMode = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // Default
	if (surfCaps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) {
		alphaCompositeMode = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	}else if (surfCaps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR) {
		alphaCompositeMode = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
	}else if (surfCaps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) {
		alphaCompositeMode = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
	}else if (surfCaps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR) {
		alphaCompositeMode = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
	}
	VkSwapchainCreateInfoKHR swapchainCI; memset(&swapchainCI, 0, sizeof(VkSwapchainCreateInfoKHR));
	swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCI.surface = surface;
	swapchainCI.minImageCount = swapchainImageCount;
	swapchainCI.imageFormat = selectedSurfaceFormat.format;
	swapchainCI.imageColorSpace = selectedSurfaceFormat.colorSpace;
	swapchainCI.imageExtent = swapchainExtent;
	swapchainCI.preTransform = surfCaps.currentTransform;
	swapchainCI.imageArrayLayers = 1; // for multiview/stereo surface, otherwise 1
	swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // image resource is exclusive to a single queue family
	swapchainCI.queueFamilyIndexCount = 0; // irrelevant if swapchainCI.imageSharingMode is not VK_SHARING_MODE_CONCURRENT
	swapchainCI.presentMode = swapchainPresentMode;
	swapchainCI.clipped = VK_TRUE; 	// Setting clipped to VK_TRUE allows the implementation to discard rendering outside of the surface area
	swapchainCI.compositeAlpha = alphaCompositeMode;
	// Image will be used as storage target in the compute shader 
	swapchainCI.imageUsage = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	// Enable transfer source on swap chain images if supported
	if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	// Enable transfer destination on swap chain images if supported
	if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	(vkCreateSwapchainKHR(logicalDevice, &swapchainCI, NULL, &swapChain)); // create a swapchain for our GPU

	(vkGetSwapchainImagesKHR(logicalDevice, swapChain, &swapchainImageCount, NULL));
	(vkGetSwapchainImagesKHR(logicalDevice, swapChain, &swapchainImageCount, swapChainImages));
	for (int i = 0; i < swapchainImageCount; i++) {
		VkImageViewCreateInfo colorAttachmentView; memset(&colorAttachmentView, 0, sizeof(VkImageViewCreateInfo));
		colorAttachmentView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		//colorAttachmentView.pNext = NULL;
		colorAttachmentView.flags = 0;
		colorAttachmentView.image = swapChainImages[i];
		colorAttachmentView.viewType = VK_IMAGE_VIEW_TYPE_2D;
		colorAttachmentView.format = selectedSurfaceFormat.format;
		VkComponentMapping componentMapping = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G,
												VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
		colorAttachmentView.components = componentMapping;
		colorAttachmentView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		colorAttachmentView.subresourceRange.baseMipLevel = 0;
		colorAttachmentView.subresourceRange.levelCount = 1;
		colorAttachmentView.subresourceRange.baseArrayLayer = 0;
		colorAttachmentView.subresourceRange.layerCount = 1;
		(vkCreateImageView(logicalDevice, &colorAttachmentView, NULL, &swapChainImageView[i]));
	}
	printf("SwapChain Images & ImageViews:\n");
	printf("\t Swapchain ImageSize: %u x %u\n", swapchainExtent.width, swapchainExtent.height);
	printf("\t Swapchain ImageCount: %d\n", swapchainImageCount);
	if (swapchainPresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
		printf("\t Swapchain Present Mode: IMMEDIATE\n");
	}
	else if (swapchainPresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
		printf("\t Swapchain Present Mode: MAILBOX\n");
	}
	else if (swapchainPresentMode == VK_PRESENT_MODE_FIFO_KHR) {
		printf("\t Swapchain Present Mode: FIFO\n");
	}
	else {
		printf("\t Swapchain Present Mode: %d\n", swapchainPresentMode);
	}
}

void initVulkan(GLFWwindow* window) {
	createInstance();
	queryPhysicalDevice_selectQueueFamily();
	queryExtensions_createLogicalDevice_getQueue();
	createSurface(window);
	createSwapChain_Images_ImageViews();
}
