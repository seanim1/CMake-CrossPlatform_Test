#include "VulkanSynchronization.h"

VulkanSynchronization::VulkanSynchronization(VkDevice logicalDevice)
{
	VkFenceCreateInfo fenceCreateInfo; memset(&fenceCreateInfo, 0, sizeof(VkFenceCreateInfo));
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // created in signaled state
	VkSemaphoreCreateInfo semaphoreCreateInfo; memset(&semaphoreCreateInfo, 0, sizeof(VkSemaphoreCreateInfo));
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		CATCH_ERROR(vkCreateFence(logicalDevice, &fenceCreateInfo, NULL, &inFlightFence[i]));
		CATCH_ERROR(vkCreateSemaphore(logicalDevice, &semaphoreCreateInfo, NULL, &presentFinImgAvailSem[i]));
		CATCH_ERROR(vkCreateSemaphore(logicalDevice, &semaphoreCreateInfo, NULL, &renderFinishedSem[i]));
	}
}
