#include "VulkanSwapChain.h"

VulkanSwapChain::VulkanSwapChain(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkDevice logicalDevice)
{
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
	SDL_Log("Supported Physical Device Surface Formats (%d count):", formatCount);
	for (int i = 0; i < formatCount; i++) {
		SDL_Log("\t%d: Format: %d, ColorSpace: %d", i, surfaceFormats[i].format, surfaceFormats[i].colorSpace);
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
		SDL_Log("\tSelected Surface Image Format: VK_FORMAT_B8G8R8A8_UNORM");
		break;
	case 1:
		SDL_Log("\tSelected Surface Image Format: VK_FORMAT_R8G8B8A8_UNORM");
		break;
	case 2:
		SDL_Log("\tSelected Surface Image Format: VK_FORMAT_A8B8G8R8_UNORM_PACK32");
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
	}
	else if (surfCaps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR) {
		alphaCompositeMode = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
	}
	else if (surfCaps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) {
		alphaCompositeMode = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
	}
	else if (surfCaps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR) {
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
	SDL_Log("SwapChain Images & ImageViews:");
	SDL_Log("\t Swapchain ImageSize: %u x %u", swapchainExtent.width, swapchainExtent.height);
	SDL_Log("\t Swapchain ImageCount: %d", swapchainImageCount);

	if (swapchainPresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
		SDL_Log("\t Swapchain Present Mode: IMMEDIATE");
	}
	else if (swapchainPresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
		SDL_Log("\t Swapchain Present Mode: MAILBOX");
	}
	else if (swapchainPresentMode == VK_PRESENT_MODE_FIFO_KHR) {
		SDL_Log("\t Swapchain Present Mode: FIFO");
	}
	else {
		SDL_Log("\t Swapchain Present Mode: %d", swapchainPresentMode);
	}
}
