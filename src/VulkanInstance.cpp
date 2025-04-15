#include "VulkanInstance.h"

VulkanInstance::VulkanInstance(std::vector<const char*>* requestedInstanceExtensions, std::vector<const char*>* requestedInstanceLayers)
{
	uint32_t apiVersion = 0;
	vkEnumerateInstanceVersion(&apiVersion);
	SDL_Log("Vulkan API version supported: %d.%d.%d",
		VK_VERSION_MAJOR(apiVersion),
		VK_VERSION_MINOR(apiVersion),
		VK_VERSION_PATCH(apiVersion));
	SDL_Log("Vulkan Instance:\n");
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
	SDL_Log("Supported Instance Extensions ( %d  count ):", extCount);
	for (uint32_t i = 0; i < extCount; i++) {
		SDL_Log("\t%d: %s", i, supportedInstanceExtensions[i].extensionName);
	}

	// Filter out unsupported extensions
	std::vector<const char*> enabledInstanceExtensions;
	for (const char* extension : *requestedInstanceExtensions) {
		bool found = false;
		for (const auto& supportedExtension : supportedInstanceExtensions) {
			if (strcmp(extension, supportedExtension.extensionName) == 0) {
				enabledInstanceExtensions.push_back(extension);
				SDL_Log("\t Instance Extension '%s' is supported.\n", extension);
				found = true;
				break;
			}
		}
		if (!found) {
			SDL_Log("\t Instance Extension '%s' is NOT supported.\n", extension);
		}
	}

	uint32_t layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	SDL_Log("Supported Validation Layers (%d count):", layerCount);
	for (uint32_t i = 0; i < layerCount; i++) {
		const VkLayerProperties& layer = availableLayers[i];
		SDL_Log("\t%d: %s", i, layer.layerName);
		SDL_Log("\t\tSpec Version: %u", layer.specVersion);
		SDL_Log("\t\tImplementation Version: %u", layer.implementationVersion);
		SDL_Log("\t\tDescription: %s", layer.description);
	}

	std::vector<const char*> enabledInstanceLayers;
	for (const char* layer : *requestedInstanceLayers) {
		bool found = false;
		for (const auto& supportedLayer : availableLayers) {
			if (strcmp(layer, supportedLayer.layerName) == 0) {
				enabledInstanceLayers.push_back(layer);
				SDL_Log("\t Instance Layer '%s' is supported.\n", layer);
				found = true;
				break;
			}
		}
		if (!found) {
			SDL_Log("\t Instance Layer '%s' is NOT supported.\n", layer);
		}
	}
	VkInstanceCreateInfo instanceCreateInfo; memset(&instanceCreateInfo, 0, sizeof(instanceCreateInfo));
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pNext = NULL;
#if defined(__APPLE__)
	// Enable portability enumeration bit
	instanceCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif
	// It appears that if Validation layer isn't found, vkCreateInstance fails and apps crash.
	instanceCreateInfo.pApplicationInfo = (VkApplicationInfo*)&appInfo;
	instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(enabledInstanceLayers.size());
	instanceCreateInfo.ppEnabledLayerNames = enabledInstanceLayers.data();
	instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledInstanceExtensions.size());
	instanceCreateInfo.ppEnabledExtensionNames = enabledInstanceExtensions.data();

	VkResult result = vkCreateInstance(&instanceCreateInfo, NULL, &instance);
	if (result != VK_SUCCESS) {
		SDL_Log("Failed vkCreateInstance! Error code: %d\n", result);
	}
	SDL_Log("Success creating vkCreateInstance\n");
}
void VulkanInstance::initSurface(SDL_Window* window) {
	// Create a platform agnostic surface
	bool result = SDL_Vulkan_CreateSurface(window, instance, nullptr, &surface);
	if (!result) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Vulkan_CreateSurface Error: %s", SDL_GetError());
	}
}